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

#include <QtGlobal>
#include <QObject>
#include <QMutex>

#ifdef Q_OS_UNIX
 #include "linux/mdtSerialPortPosix.h"
 #define mdtSerialPortSys mdtSerialPortPosix
#endif

class mdtSerialPort : public mdtSerialPortSys
{
 Q_OBJECT

 public:

  mdtSerialPort(QObject *parent = 0);
  ~mdtSerialPort();

  /*! \brief Open the serial port with given configuration
   * \param cfg Contains the setup for the serial port to open
   * \return True on successfull configuration and open port
   */
  //bool open(mdtSerialPortConfig &cfg);

  /*! \brief Get the CAR (CD) state
   * \SA Signals provided in mdtAbstractSerialPort
   */
  bool carIsOn();

  /*! \brief Get the DSR state
   * \SA Signals provided in mdtAbstractSerialPort
   */
  bool dsrIsOn();

  /*! \brief Get the CTS state
   * \SA Signals provided in mdtAbstractSerialPort
   */
  bool ctsIsOn();

  /*! \brief Get the RNG (RI) state
   * \SA Signals provided in mdtAbstractSerialPort
   */
  bool rngIsOn();

 public slots:

  /*! \brief Enable/diseable the RTS (Request To Send) signal
   * \param on If true, RTS will be enabled, diseabled else
   */
  void setRts(bool on);

  /*! \brief Enable/diseable the DTR (Data Terminal Ready) signal
   * \param on If true, DTR will be enabled, diseabled else
   */
  void setDtr(bool on);
  
 private:

  QMutex pvMutex;
};

#endif  // #ifndef MDT_SERIAL_PORT_H
