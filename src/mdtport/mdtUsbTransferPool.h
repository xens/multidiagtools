/****************************************************************************
 **
 ** Copyright (C) 2011-2015 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_USB_TRANSFER_POOL_H
#define MDT_USB_TRANSFER_POOL_H

#include "mdtUsbEndpointDescriptor.h"
#include <QtGlobal>
#include <vector>
#include <algorithm>
#include <libusb-1.0/libusb.h>

/*! \brief Storage of USB transfers
 *
 * This class handles 2 transfer queues:
 *  - Pool: available transfers to initiate I/O requests
 *  - Pending: transfers that where submitted but not yet completed
 *
 * To get a transfer, use getTransfer(). This will directly
 *  put returned transfer to pending (but not yet completed) queue.
 *
 * Because submitting a transfer, and cancelling the same transfer,
 *  can be done in concurrent way, a lock should be used to prevent this.
 *
 *  For example, submitting could be something like:
 *   1) Lock mutex
 *   2) transfer = getTransfer(); // Transfer will immediatly be available in pending queue.
 *   3) Fill transfer
 *   4) Submit transfer
 *   5) Unlock mutex
 *
 * Cancel function could be like:
 *   1) Lock mutex
 *   2) transfer = getPendingTransfer();
 *   3) Cancel transfer (submition only, libusb_cancel_transfer() will return before completion)
 *   4) Unlock mutex
 *
 * Finally, the transfer callback could be like:
 *   1) Lock mutex
 *   2) restoreTransfer()
 *   3) Unlock mutex
 *   4) If required, submit a new transfer (whenn the mutex is unlocked!)
 *
 * Because several tasks must be done in a atomic way, it was chosen to not put any lock in this class.
 */
template<typename T>
class mdtUsbTransferPool
{
 public:

  /*! \brief Constructor
   *
   * \param maxTransfers Maximum allowed number of transfers.
   *           This limit can be usefull for bug detection.
   *           If exceeded, getTransfer() will return a null pointer.
   *           In debug build, a assertion will also fail in this case.
   */
  mdtUsbTransferPool(unsigned int maxTransfers = 50)
   : pvDeviceHandle(0),
     pvMaxTransfers(maxTransfers),
     pvAllocatedTransfers(0)
  {
  }

  /*! \brief Destructor
   */
  ~mdtUsbTransferPool()
  {
    clear();
  }

  /*! \brief Clear pool
   *
   * Will also destroy each transfer.
   */
  void clear()
  {
    restorePendingTransfers();
    Q_ASSERT(pvTransfers.size() == pvAllocatedTransfers);
    for(T *transfer : pvTransfers){
      delete transfer;
    }
    pvAllocatedTransfers = 0;
    pvTransfers.clear();
    pvDeviceHandle = 0;
  }

  /*! \brief Alloc transfers
   *
   * \param n Number of transfers
   * \param th Application specific transfer handler
   * \param devHandle Must be a valid pointer to currently open device.
   * \pre n must be <= maxTransfers (given in constructor)
   */
  template<typename TransferHandlerType>
  bool allocTransfers(unsigned int n, TransferHandlerType & th, libusb_device_handle *devHandle)
  {
    Q_ASSERT(devHandle != 0);
    Q_ASSERT(n <= pvMaxTransfers);

    transferAllocator<TransferHandlerType> ta(th, devHandle);

    return allocTransfers<TransferHandlerType>(n, ta);
  }

  /*! \brief Alloc transfers
   *
   * \param n Number of transfers
   * \param th Application specific transfer handler
   * \param devHandle Must be a valid pointer to currently open device.
   * \param endpointDescriptor USB endpoint descriptor.
   * \pre n must be <= maxTransfers (given in constructor)
   */
  template<typename TransferHandlerType>
  bool allocTransfers(unsigned int n, TransferHandlerType & th, mdtUsbEndpointDescriptor endpointDescriptor, libusb_device_handle *devHandle)
  {
    Q_ASSERT(devHandle != 0);
    Q_ASSERT(n <= pvMaxTransfers);

    transferAllocatorEpd<TransferHandlerType> ta(th, endpointDescriptor, devHandle);

    return allocTransfers<TransferHandlerType>(n, ta);
  }

  /*! \brief Get a transfer from pool
   *
   * Will create a new transfer if pool is empty.
   *
   * The transfer is also filed into pending queue.
   *
   * \return A valid pointer to transfer (or 0 on memory allocation failure)
   */
  template<typename TransferHandlerType>
  T *getTransfer(TransferHandlerType & th)
  {
    Q_ASSERT(pvDeviceHandle != 0);

    transferAllocator<TransferHandlerType> ta(th, pvDeviceHandle);

    return getTransfer<TransferHandlerType>(ta);
  }

  /*! \brief Get a transfer from pool
   *
   * Will create a new transfer if pool is empty.
   *
   * The transfer is also filed into pending queue.
   *
   * \return A valid pointer to transfer (or 0 on memory allocation failure)
   */
  template<typename TransferHandlerType>
  T *getTransfer(TransferHandlerType & th, mdtUsbEndpointDescriptor endpointDescriptor)
  {
    Q_ASSERT(pvDeviceHandle != 0);

    transferAllocatorEpd<TransferHandlerType> ta(th, endpointDescriptor, pvDeviceHandle);

    return getTransfer<TransferHandlerType>(ta);
  }

  /*! \brief Restore a transfer
   */
  void restoreTransfer(T *transfer)
  {
    Q_ASSERT(transfer != 0);
    // Restore to pool
    Q_ASSERT(std::find(pvTransfers.begin(), pvTransfers.end(), transfer) == pvTransfers.end()); // Check that pool not allready contains transfer
    pvTransfers.push_back(transfer);
    // Remove from pending queue
    auto it = std::find(pvPendingTransfers.begin(), pvPendingTransfers.end(), transfer);
    Q_ASSERT(it != pvPendingTransfers.end()); // transfer must exist in pending queue
    pvPendingTransfers.erase(it);
  }

  /*! \brief Get number of pending transfers
   */
  int pendingTransferCount() const{
    return pvPendingTransfers.size();
  }

  /*! \brief Get a pending transfer
   *
   * Will return 0 if no transfer is pending.
   *  This function will simply return the last
   *  transfer in pending queue, and not remove it.
   *  Removal from pending queue is done by restoreTransfer()
   *  or restorePendingTransfers().
   */
  T *getPendingTransfer(){
    if(pvPendingTransfers.empty()){
      return 0;
    }
    return pvPendingTransfers.back();
  }

  /*! \brief Get all pending transfers
   *
   * Note that returned transfers are not removed
   *  from pending queue. Be aware that another thread,
   *  or a callback, could restore a transfer at the same time,
   *  and that will invalidate returned queue.
   */
  std::vector<T*> getAllPendingTransfers()
  {
    return pvPendingTransfers;
  }

  /*! \brief Restore pending transfers to pool
   */
  void restorePendingTransfers(){
    for(T *transfer : pvPendingTransfers){
      pvTransfers.push_back(transfer);
    }
    pvPendingTransfers.clear();
  }

 private:

  /*! \brief Allocator for transfer class without endpoint descriptor
   */
  template<typename TransferHandlerType>
  class transferAllocator
  {
   public:
    // Constructor
    transferAllocator(TransferHandlerType & th, libusb_device_handle *handle)
     : deviceHandle(handle),
       pvTh(th)
    {
    }
    // Allocation function
    T *allocateNewTransfer() const
    {
      return new T(pvTh, deviceHandle);
    }
    // Members
    libusb_device_handle *deviceHandle;
   private:
    TransferHandlerType & pvTh;
  };

  /*! \brief Allocator for transfer class with endpoint descriptor
   */
  template<typename TransferHandlerType>
  class transferAllocatorEpd
  {
   public:
    // Constructor
    transferAllocatorEpd(TransferHandlerType & th, mdtUsbEndpointDescriptor d, libusb_device_handle *handle)
     : deviceHandle(handle),
       pvTh(th),
       pvD(d)
    {
    }
    // Allocation function
    T *allocateNewTransfer() const
    {
      return new T(pvTh, pvD, deviceHandle);
    }
    // Members
    libusb_device_handle *deviceHandle;
   private:
    TransferHandlerType & pvTh;
    mdtUsbEndpointDescriptor pvD;
  };

  /*! \brief Alloc transfers
   *
   * \param n Number of transfers
   * \param allocator Allocator
   */
  template<typename TransferHandlerType, typename A>
  bool allocTransfers(unsigned int n, const A & allocator)
  {
    Q_ASSERT(n <= pvMaxTransfers);

    unsigned int i;
    T *transfer;

    clear();
    pvDeviceHandle = allocator.deviceHandle;
    pvTransfers.reserve(n);
    for(i = 0; i < n; ++i){
      transfer = allocator.allocateNewTransfer();
      if(transfer == 0){
        clear();
        return false;
      }
      pvTransfers.push_back(transfer);
    }
    pvAllocatedTransfers = pvTransfers.size();

    return true;
  }

  /*! \brief Get a transfer from pool
   */
  template<typename TransferHandlerType, typename A>
  T *getTransfer(const A & allocator)
  {
    Q_ASSERT(pvDeviceHandle != 0);

    T *transfer;

    // Allocate a new transfer if required and permitted
    if(pvTransfers.empty()){
      Q_ASSERT(pvAllocatedTransfers < pvMaxTransfers);
      if(pvAllocatedTransfers < pvMaxTransfers){
        ++pvAllocatedTransfers;
        transfer = allocator.allocateNewTransfer();
        pvPendingTransfers.push_back(transfer);
        return transfer;
      }else{
        return 0;
      }
    }
    // Have in pool, get one
    transfer = pvTransfers.back();
    Q_ASSERT(transfer != 0);
    pvTransfers.pop_back();
    pvPendingTransfers.push_back(transfer);

    return transfer;
  }

  Q_DISABLE_COPY(mdtUsbTransferPool);

  std::vector<T*> pvTransfers;
  std::vector<T*> pvPendingTransfers;
  libusb_device_handle *pvDeviceHandle;
  unsigned int pvMaxTransfers;
  unsigned int pvAllocatedTransfers;
};

#endif // #ifndef MDT_USB_TRANSFER_POOL_H
