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
#ifndef MDT_DEVICE_DSO1000A_H
#define MDT_DEVICE_DSO1000A_H

#include "mdtDeviceScpi.h"

class mdtDeviceDSO1000A : public mdtDeviceScpi
{
 Q_OBJECT

 public:

  /*! \brief Construct a DSO1000A device
   */
  mdtDeviceDSO1000A(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtDeviceDSO1000A();

  /*! \brief Connect to device
   *
   * \param serialNumber Device serial number. Will be ignored if empty
   */
  bool connectToDevice(const QString & serialNumber = QString())
  {
    return mdtDeviceScpi::connectToDevice(0x0957, 0x0588, serialNumber);
  }

private:

  Q_DISABLE_COPY(mdtDeviceDSO1000A);

};

#endif  // #ifndef MDT_DEVICE_DSO1000A_H
