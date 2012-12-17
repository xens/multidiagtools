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
#ifndef MDT_USB_PORT_MANAGER_H
#define MDT_USB_PORT_MANAGER_H

#include "mdtPortManager.h"
#include "mdtFrameUsbControl.h"

/*! \brief Helper class that simplify access to USB port API
 *
 * Based on mdtPortManager, some USB specific methods are added.
 *  The internal configuration (a mdtPortConfig object)
 *  is keeped default.
 */
class mdtUsbPortManager : public mdtPortManager
{
 Q_OBJECT

 public:

  /*! \brief Contruct a USB port manager
   *
   * Will create mdtUsbPort and mdtUsbPortThread objects.
   */
  mdtUsbPortManager(QObject *parent = 0);

  /*! \brief Destructor
   *
   * Stop the manager (if running), and close the port (if open).
   *  All internal objects (port, config and threads) are deleted here.
   */
  virtual ~mdtUsbPortManager();

  /*! \brief Get a list of attached USB devices
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   *
   * \pre Manager must no running
   */
  virtual QList<mdtPortInfo*> scan();

  /*! \brief Get a list of attached USB devices
   *
   * Note that returned list must be freed by user
   *  after usage. (for.ex. with qDeletAll() and QList::clear() ).
   *
   * \param bDeviceClass If >= 0, only devices with matching bDeviceClass are listed
   * \param bDeviceSubClass If >= 0, only devices with matching bDeviceSubClass are listed
   * \param bDeviceProtocol If >= 0, only devices with matching bDeviceProtocol are listed
   *
   * \pre Manager must no running
   */
  QList<mdtPortInfo*> scan(int bDeviceClass, int bDeviceSubClass, int bDeviceProtocol);

  /*! \brief Send a control request by copy
   *
   * Request will be passed to the mdtPort's control request queue by copy.
   *  This method returns immediatly after enqueue,
   *  and don't wait until data was written.
   *
   * \param frame Frame containing request to send.
   * \return 0 on success or value < 0 on error. In this implementation,
   *          the only possible error is mdtAbstractPort::WriteQueueEmpty .
   *          Some subclass can return a frame ID on success,
   *          or a other error. See subclass documentation for details.
   * \pre Port must be set with setPort() with a mdtUsbPort (or subclass) before use of this method.
   *
   *
   * Subclass notes:<br>
   *  This method can be reimplemented in subclass if needed.
   *  Typically usefull if a frame ID must be generated for each request (f.ex. bTag on USBTMC).
   *  A frame must be taken from port's request frames pool with mdtAbstractPort::controlFramesPool()
   *  dequeue() method (see Qt's QQueue documentation for more details on dequeue() ),
   *  then added to port's request queue with mdtAbstractPort::addControlRequest() .
   *  If protocol supports frame identification (like USBTMC's bTag),
   *   it should be returned here and incremented.
   */
  virtual int sendControlRequest(const mdtFrameUsbControl &request);

 public slots:

  void fromThreadControlResponseReaden();

  void fromThreadMessageInReaden();

 private:

  Q_DISABLE_COPY(mdtUsbPortManager);
};

#endif  // #ifndef MDT_USB_PORT_MANAGER_H
