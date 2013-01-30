/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#ifndef MDT_USBTMC_PORT_MANAGER_H
#define MDT_USBTMC_PORT_MANAGER_H

#include "mdtUsbPort.h"
#include "mdtUsbtmcPortThread.h"
#include "mdtUsbPortManager.h"
#include "mdtPortInfo.h"
#include <QObject>
#include <QStringList>
#include <QList>

/// \bug abortBulkIn() does not work properly
/// \todo Write a USBTMC port thread

/*! \brief USBTMC port manager
 *
 * This is the easiest way to use the USBTMC port API.
 *
 * All needed object are created by constructor.
 *  To alter configuration, use config().
 *  To access the usb port object, use port().
 *
 * Use start() to begin read/write and stop to end.
 */
class mdtUsbtmcPortManager : public mdtUsbPortManager
{
 Q_OBJECT

 public:

  /*! \brief Construct a USBTMC port manager
   *
   * Create a thread and setup port.
   */
  mdtUsbtmcPortManager(QObject *parent = 0);

  /*! \brief Destructor
   *
   * Stop the manager (if running), and close the port (if open).
   *  All internal objects (port, config and threads) are deleted here.
   */
  ~mdtUsbtmcPortManager();

  /*! \brief Scan for available ports with a USBTMC compatible device attached
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   *
   * \pre Manager must no running
   */
  QList<mdtPortInfo*> scan();

  /*! \brief Send a command to device
   *
   * Wait until it's possible to write to device,
   *  then send the command.
   *
   * Note that the wait will not break the GUI's event loop
   *  (see mdtPortManager::waitOnWriteReady() for details).
   *
   * \param command Command to send.
   * \param timeout Write timeout [ms]
   *                 If 0, internal defined timeout is used (see mdtPortManager::adjustedReadTimeout() for details).
   * \return bTag on success, or a error < 0
   *          (see mdtUsbtmcPortManager::writeData() for details).
   */
  int sendCommand(const QByteArray &command, int timeout = 0);

  /*! \brief Send a query to device
   *
   * Wait until it's possible to write to device,
   *  send the query and wait until a response
   *  is available or timeout.
   *
   * Note that the wait will not break the GUI's event loop.
   *
   * \param query Query to send.
   * \param writeTimeout Write timeout [ms]
   *                 If 0, internal defined timeout is used (see mdtPortManager::adjustedWriteTimeout() for details).
   * \param readTimeout Response timeout [ms]
   *                 If 0, internal defined timeout is used (see mdtPortManager::adjustedReadTimeout() for details).
   * \return Result as string (empty string on error)
   *          Note that mdtFrameCodecScpi can be helpful to decode returned result.
   */
  QByteArray sendQuery(const QByteArray &query, int writeTimeout = 0, int readTimeout = 0);

  /*! \brief Write data by copy
   *
   * Data will be encoded regarding USBTMC standard and passed to the mdtUsbPort's write queue by copy.
   *  This method returns immediatly after enqueue,
   *  and don't wait until data was written.
   *
   * \param data Data to write
   * \return bTag ID on success or mdtAbstractPort::WriteQueueEmpty (< 0).
   * \pre Port must be set with setPort() before use of this method.
   */
  int writeData(QByteArray data);

  /*! \brief Send a read request to device
   *
   * USBTMC standard need that a read request is sent to device
   *  before we can read any data.
   *
   * \param transaction A pointer to a valid transaction, with setQueryReplyMode set.
   *                     (id will be set internally).
   * \return bTag ID on success or value < 0 if write queue is full.
   * \pre port must be set
   * \pre transaction must be a valid pointer
   */
  int sendReadRequest(mdtPortTransaction *transaction);

  /*! \brief Send a read request to device
   *
   * USBTMC standard need that a read request is sent to device
   *  before we can read any data.
   *
   * \param enqueueResponse When data comes in, \todo SigName is emited.
   *                         In blocking mode, data must (additionally) be enqueued
   *                         until there are readen. So, for blocking mode ("script"),
   *                         set this parameter true.
   * \return bTag ID on success or value < 0 if write queue is full.
   */
  int sendReadRequest(bool enqueueResponse);

  /*! \brief Send a READ_STATUS_BYTE request thru the control endpoint
   *
   * \return bTag on success or WriteQueueEmpty on error.
   */
  int sendReadStatusByteRequest();

  /*! \brief Abort bulk IN
   *
   * \return 0 on success.
   * \pre port must be set before call of this method
   */
  ///int abortBulkIn(quint8 bTag);
  ///void abortBulkIn(quint8 bTag);

  /*! \brief Send a INITIATE_ABORT_BULK_IN request thru the control endpoint
   *
   * \see abortBulkIn()
   * \return bTag on success or WriteQueueEmpty on error.
   */
  ///int sendInitiateAbortBulkInRequest(quint8 bTag);

  /*! \brief Send a CHECK_ABORT_BULK_IN_STATUS request thru the control endpoint
   *
   * \see abortBulkIn()
   * \return bTag on success or WriteQueueEmpty on error.
   */
  ///int sendCheckAbortBulkInStatusRequest(quint8 bTag);

  /*! \brief Abort bulk OUT
   *
   * \return 0 on success.
   * \pre port must be set before call of this method
   */
  int abortBulkOut(quint8 bTag);

  /*! \brief Send a INITIATE_ABORT_BULK_OUT request thru the control endpoint
   *
   * \see abortBulkOut()
   * \return bTag on success or WriteQueueEmpty on error.
   */
  int sendInitiateAbortBulkOutRequest(quint8 bTag);

  /*! \brief Send a CHECK_ABORT_BULK_OUT_STATUS request thru the control endpoint
   *
   * \see abortBulkOut()
   * \return bTag on success or WriteQueueEmpty on error.
   */
  int sendCheckAbortBulkOutStatusRequest(quint8 bTag);

 public slots:

  /*! \brief Called by the thread whenn a frame was readen
   */
  void fromThreadNewFrameReaden();

  /*! \brief Manage errors comming from port threads
   */
  void onThreadsErrorOccured(int error);

 private:

  quint8 pvCurrentWritebTag;  // USBTMC's frame bTag

  // Diseable copy
  Q_DISABLE_COPY(mdtUsbtmcPortManager);
};

#endif  // #ifndef MDT_USBTMC_PORT_MANAGER_H
