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
#ifndef MDT_SERIAL_PORT_POSIX_H
#define MDT_SERIAL_PORT_POSIX_H

#include "mdtAbstractSerialPort.h"
#include "mdtSerialPortConfig.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <pthread.h>
#include <QObject>

/*
 * Ressources: http://www.easysw.com/~mike/serial/serial.html
 * 
 */

class mdtSerialPortPosix : public mdtAbstractSerialPort
{
 public:

  mdtSerialPortPosix(QObject *parent = 0);
  ~mdtSerialPortPosix();

  // Implemented from mdtAbstractPort
  bool setAttributes(const QString &portName);

  // Implemented from mdtAbstractSerialPort
  bool open(mdtSerialPortConfig &cfg);

  // Restore original setup and close the port
  void close();

  // Implemtation of mdtAbstractPort
  void setReadTimeout(int timeout);

  // Implemtation of mdtAbstractPort
  void setWriteTimeout(int timeout);

  // Implemtation of mdtAbstractPort
  bool waitForReadyRead();

  // Implemtation of mdtAbstractPort method
  qint64 read(char *data, qint64 maxSize);

  // Re-implementation of mdtAbstractPort method
  bool suspendTransmission();

  // Re-implementation of mdtAbstractPort method
  bool resumeTransmission();

  // Implemtation of mdtAbstractPort method
  void flushIn();

  // Implemtation of mdtAbstractPort
  bool waitEventWriteReady();

  // Implemtation of mdtAbstractPort method
  qint64 write(const char *data, qint64 maxSize);

  // Implemtation of mdtAbstractPort method
  void flushOut();

  // Wait until a control signal (modem line state) changes
  bool waitEventCtl();

  // Get the control signal (modem line) states and update member flags
  bool getCtlStates();

  void setRts(bool on);

  void setDtr(bool on);

  // Must be called from signal control thread (see mdtSerialPortCtlThread)
  void defineCtlThread(pthread_t ctlThread);

  // Abort the waitEventCtl() function
  // Note: the thread in witch the waitEventCtl() function
  //       is called must be defined with defineCtlThread() before
  void abortWaitEventCtl();

 private:

  // Set the baud rate. Returns false if baud rate is not supported
  bool setBaudRate(int rate);

  // Get configured baud rate
  int baudRate();

  // Set number of data bits. Returns false on un unsupported number
  bool setDataBits(int n);

  // Get configured number of data bits
  int dataBits();

  // Set number of stop bits. Returns false on un unsupported number
  bool setStopBits(int n);

  // Get configured number of stop bits
  int stopBits();

  //  Set parity check
  void setParity(mdtSerialPortConfig::parity_t p);

  // Get configured parity
  mdtSerialPortConfig::parity_t parity();

  // Enable/diseable RTS/CTS flow control
  void setFlowCtlRtsCts(bool on);

  // Returns true if RTS/CTS flow control is enabled
  bool flowCtlRtsCtsOn();

  // Enable/diseable Xon/Xoff flow control
  void setFlowCtlXonXoff(bool on, char xonChar, char xoffChar);

  // Returns true if Xon/Xoff flow control is enabled
  bool flowCtlXonXoffOn();

  // Check if configuration could be done on system
  bool checkConfig(mdtSerialPortConfig cfg);
  
  // Used to catch the SIGALRM signal (doese nothing else)
  static void sigactionHandle(int signum);

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
  // Members used for control signals thread kill
  pthread_t pvCtlThread;
  struct sigaction pvSigaction;
  bool pvAbortingWaitEventCtl;
  int pvFd;                         // Port file descriptor
  struct timeval pvReadTimeout;
  struct timeval pvWriteTimeout;

};

#endif  // #ifndef MDT_SERIAL_PORT_POSIX_H
