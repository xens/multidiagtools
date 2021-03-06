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
#ifndef MDT_DEVICE_SCPI_H
#define MDT_DEVICE_SCPI_H

#include "mdtDevice.h"
#include "mdtError.h"
#include "mdtUsbtmcPort.h"
#include "mdtCodecScpi.h"
#include "mdtValue.h"
#include <QList>
#include <QVariant>
#include <cstdint>

#include <QObject>

class QTimer;

/*! \brief Base class for SCPI based instruments
 *
 * Most of current instruments (Agilent, NI, ...)
 *  support SCPI command format.
 *
 * This class give some common functions
 *  requierd to handle such devices.
 *
 * Currently, only USBTMC port is supported.
 */
class mdtDeviceScpi : public mdtDevice
{
 Q_OBJECT

 public:

  /*! \brief Construct a SCPI device
   *
   * Will create and configure a USBTMC
   *  port (mdtUsbtmcPort instance).
   *
   * Default transaction timeout will also be set to 30000ms (30s)
   */
  mdtDeviceScpi(QObject *parent = 0);

  /*! \brief Destructor
   */
  virtual ~mdtDeviceScpi();

  /*! \brief Set default transaction timeout
   *
   * \param timeout Timeout [ms]
   */
  void setDefaultTimeout(int timeout)
  {
    pvDefaultTimeout = timeout;
  }

  /*! \brief Get default transaction timeout
   */
  inline int defaultTimeout() const
  {
    return pvDefaultTimeout;
  }

  /*! \brief Connect to a USBTMC device
   *
   * Connect to device that was set with setDeviceAddress()
   *
   * \sa mdtDevice::setDeviceAddress().
   */
  bool connectToDevice();

  /*! \brief Connect to a USBTMC device
   *
   * \param idVendor Vendor ID
   * \param idProduct Product ID
   * \param serialNumber Device serial number. Will be ignored if empty
   * \param alias Alias to give. Will be ignored if empty
   */
  bool connectToDevice(uint16_t idVendor, uint16_t idProduct, const QString & serialNumber, const QString & alias = QString());

  /*! \brief Send a command to device
   *
   * \param command Command to send
   * \param timeout Timeout [ms]. If -2 is given, defaultTimeout will be used.
   */
  bool sendCommand(const QByteArray & command, int timeout = -2);

  /*! \brief Send a query to device
   *
   * \param query Query to send
   * \param timeout Timeout [ms]. If -2 is given, defaultTimeout will be used.
   */
  QByteArray sendQuery(const QByteArray & query, int timeout = -2);

  /*! \brief Get a double value for given query
   */
  mdtValueDouble getValueDouble(const QByteArray & query, int timeout = -2);

  /*! \brief Get device identification
   *
   * Will use *IDN query to get device identification.
   *  On error, a null IDN response is returned.
   *
   */
  mdtScpiIdnResponse getDeviceIdentification();

  /*! \brief Get device error
   *
   * Will send SYSTem:ERRor? query and return error.
   *  On (communication) problem, error level will be set to NoError.
   *
   * \param errorText Text to set in error. If empty, a default text will be set.
   */
  mdtError getDeviceError(const QString & errorText = QString());

  /*! \brief Send a command to device
   *
   * Wait until it's possible to write to device,
   *  then send the command.
   *
   * Note that the wait will not break the GUI's event loop
   *  (see mdtPortManager::waitOnWriteReady() for details).
   *
   * This method will not update internal device state.
   *  It's recommended to use mdtDevice functions if available.
   *
   * \param command Command to send.
   * \return bTag on success, or a error < 0
   *          (see mdtUsbtmcPortManager::writeData() for details).
   */
  ///int sendCommand(const QByteArray &command);

  /*! \brief Send a query to device
   *
   * Wait until it's possible to write to device,
   *  send the query and wait until a response
   *  is available or timeout.
   *
   * Note that the wait will not break the GUI's event loop.
   *
   * This method will not update internal device and I/O states.
   *  It's recommended to use mdtDevice functions if available.
   *
   * \param query Query to send.
   * \return Result as string (empty string on error)
   *          Note that mdtFrameCodecScpi can be helpful to decode returned result.
   */
  ///QByteArray sendQuery(const QByteArray &query);

  /*! \brief Wait until operation is complete (Using *OPC? query)
   *
   * Currently, this method queries device with *OPC?.
   *
   * \param timeout Timeout [ms]
   * \param interval Interval between 2 *OPC? query [ms]
   * \pre interval > 0
   */
  bool waitOperationComplete(int timeout, int interval);

 protected:

  /*! \brief Close port
   */
  void disconnectFromDeviceEvent();

  /*! \brief Update name in port
   */
  void deviceAddressChangedEvent(const mdtDeviceAddress & address);

 private slots:

  /*! \brief Query device about operation complete
   *
   * The first call of this slot is made by waitOperationComplete(),
   *  then it will be called by internal timer until operation is complete,
   *  or pvOperationCompleteTryLeft is < 1.
   *
   * If operation is complete, pvOperationComplete is set to true.
   */
  void queryAboutOperationComplete();

 private:

  mdtUsbtmcPort *pvPort;
  int pvDefaultTimeout; // Default transaction timeout [ms]

  bool pvOperationComplete;
  int pvOperationCompleteTryLeft;
  QTimer *pvOperationCompleteTimer;
};

#endif  // #ifndef MDT_DEVICE_SCPI_H
