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
#ifndef MDT_SERIAL_PORT_H
#define MDT_SERIAL_PORT_H

#include "mdtAbstractSerialPort.h"
#include "mdtSerialPortConfig.h"
#include "linux/mdtPortLock.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <QObject>

/*
 * Ressources: http://www.easysw.com/~mike/serial/serial.html
 * 
 */

class mdtSerialPort : public mdtAbstractSerialPort
{
 public:

  mdtSerialPort(QObject *parent = 0);
  ~mdtSerialPort();

  /*! \brief Set the baud rate
   *
   * The mutex is not handled by this method.
   *
   * \return True if baud rate is supported and could be set.
   */
  bool setBaudRate(int rate);

  /*! \brief Get the baud rate
   *
   * The mutex is not handled by this method.
   *
   * \return Configured baud rate, or 0 on error.
   */
  int baudRate();

  /*! \brief Set number of data bits.
   *
   * The mutex is not handled by this method.
   *
   * \return True if data bits count is supported and could be set.
   */
  bool setDataBits(int n);

  /*! \brief Get number of data bits.
   *
   * The mutex is not handled by this method.
   *
   * \return Configured data bits count or 0 on error.
   */
  int dataBits();

  /*! \brief Set number of stop bits.
   *
   * The mutex is not handled by this method.
   *
   * \return True if stop bits count is supported and could be set.
   */
  bool setStopBits(int n);

  /*! \brief Get number of stop bits.
   *
   * The mutex is not handled by this method.
   *
   * \return Configured stop bits count or -1 on error.
   */
  int stopBits();

  /*! \brief Set parity check.
   *
   * The mutex is not handled by this method.
   *
   * \return True if parity check is supported and could be set.
   */
  bool setParity(mdtSerialPortConfig::parity_t p);

  /*! \brief Get configured parity
   *
   * The mutex is not handled by this method.
   *
   * \return Configured parity or NoParity on error.
   */
  mdtSerialPortConfig::parity_t parity();

  /*! \brief Enable/diseable RTS/CTS flow control
   *
   * The mutex is not handled by this method.
   *
   * \return True if RTS/CTS flow control is supported and could be set.
   */
  bool setFlowCtlRtsCts(bool on);

  /*! \brief Get configured state of RTS/CTS flow control
   *
   * The mutex is not handled by this method.
   *
   * \return True if RTS/CTS flow control is enabled (and no error occured).
   */
  bool flowCtlRtsCtsOn();

  /*! \brief Enable/diseable Xon/Xoff flow control
   *
   * The mutex is not handled by this method.
   *
   * \return True if Xon/Xoff flow control is supported and could be set.
   */
  bool setFlowCtlXonXoff(bool on, char xonChar, char xoffChar);

  /*! \brief Get configured state of Xon/Xoff flow control
   *
   * The mutex is not handled by this method.
   *
   * \return True if Xon/Xoff flow control is enabled (and no error occured).
   */
  bool flowCtlXonXoffOn();

  /*! \brief Set the read data timeout
   *
   * The mutex is not handled by this method.
   */
  void setReadTimeout(int timeout);

  /*! \brief Set the write data timeout
   *
   * The mutex is not handled by this method.
   */
  void setWriteTimeout(int timeout);

  /*! \brief Wait until data is available on port.
   *
   * This method is called from mdtPortReadThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   */
  error_t waitForReadyRead();

  /*! \brief Read data from port
   *
   * This method is called from mdtPortReadThread , and should not be used directly.
   *
   * Mutex is not handled by this method.
   */
  qint64 read(char *data, qint64 maxSize);

  // Re-implementation of mdtAbstractPort method
  bool suspendTransmission();

  // Re-implementation of mdtAbstractPort method
  bool resumeTransmission();

  // Implemtation of mdtAbstractPort method
  ///void flushIn();

  /*! \brief Wait until data can be written to port.
   *
   * This method is called from mdtPortWriteThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   */
  error_t waitEventWriteReady();

  // Implemtation of mdtAbstractPort method
  qint64 write(const char *data, qint64 maxSize);

  // Implemtation of mdtAbstractPort method
  ///void flushOut();

  /*! \brief Wait until a control (modem line) signal state changes
   *
   * This method is called from mdtSerialPortCtlThread , and should not be used directly.<br>
   * Mutex must be locked before calling this method with lockMutex(). The mutex is locked when method returns.
   */
  error_t waitEventCtl();

  // Get the control signal (modem line) states and update member flags
  bool getCtlStates();

  void setRts(bool on);

  void setDtr(bool on);

 private:

  /*! \brief Open the port given by setPortName()
   *
   * If port can be open successfull, NoError code is returned.
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must not be open whenn calling this method.
   *
   * \sa close()
   * \sa error_t
   */
  error_t pvOpen();

  /*! \brief Close port
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must be open whenn calling this method.
   */
  void pvClose();

  /*! \brief Setup port with given configurations.
   *
   * The mutex is not handled by this method.
   *
   * \pre The port must be open whenn calling this method.
   *
   * \sa error_t
   */
  error_t pvSetup();

  /*! \brief Flush read port
   *
   * This method is called from flushIn(),
   *  and is usefull if subsystem needs to be flushed.
   *  (For ex. serial port).
   *
   * This method must be implemented in subclass.
   *
   * The mutex is handled by flushIn() and should not
   *  be handled here.
   */
  void pvFlushIn();

  /*! \brief Flush write port
   *
   * This method is called from flushout(),
   *  and is usefull if subsystem needs to be flushed.
   *  (For ex. serial port).
   *
   * This method must be implemented in subclass.
   *
   * The mutex is handled by flushOut() and should not
   *  be handled here.
   */
  void pvFlushOut();

  /*! \brief Map the system defined UART type to internal one.
   */
  void mapUartType();

  /*! \brief Build the list of available baud rates.
   */
  void buildAvailableBaudRates();

  // Set the RTS ON/OFF
  bool setRtsOn();
  bool setRtsOff();

  struct termios pvTermios;         // Termios configuration structure
  struct termios pvOriginalTermios; // Original termios configuration structure to restore
  // Ctl signals states memory
  int pvPreviousCarState;
  int pvPreviousDsrState;
  int pvPreviousCtsState;
  int pvPreviousRngState;
  int pvFd;                         // Port file descriptor
  struct timeval pvReadTimeout;
  struct timeval pvWriteTimeout;
  mdtPortLock *pvPortLock;
};

#endif  // #ifndef MDT_SERIAL_PORT_POSIX_H
