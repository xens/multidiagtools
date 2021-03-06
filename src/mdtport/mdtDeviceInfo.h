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
#ifndef MDT_DEVICE_INFO_H
#define MDT_DEVICE_INFO_H

#include <QString>
#include <QMetaType>

/*! \brief Contains informations about a device (attached to a port)
 *
 * Used by mdtPortManager in scan() method.
 */
class mdtDeviceInfo
{
 public:

  mdtDeviceInfo();

  ~mdtDeviceInfo();

  /*! \brief Set vendor ID
   */
  void setVendorId(int vid);

  /*! \brief Get vendor ID
   */
  int vendorId() const;

  /*! \brief Get vendor name
   * 
   * \todo Conficting with mdtUsbDeviceDescriptor
   */
  QString vendorName() const;

  /*! \brief Set product ID
   */
  void setProductId(int pid);

  /*! \brief Get product ID
   */
  int productId() const;

  /*! \brief Get product name
   *
   * \todo Conficting with mdtUsbDeviceDescriptor
   */
  QString productName() const;

  /*! \brief Set serial number/ID
   */
  void setSerialId(const QString &id);

  /*! \brief Get serial number/ID
   */
  QString serialId() const;

  /*! \brief Set protocol ID
   */
  void setProtocolId(int pid);

  /*! \brief Get product ID
   */
  int protocolId() const;

  /*! \brief Set the display text
   *
   * Set a text that can be displayed to the user.
   *  (F.ex. in a combo box)
   */
  void setDisplayText(const QString &text);

  /*! \brief Get the display text
   *
   * Returns a text that can be displayed to the user.
   *  (F.ex. in a combo box)
   *
   * If text was not set, a string containing
   *  the vendor ID ans product ID is returned.
   */
  QString displayText() const;

  /*! \brief Comparaison operator
   *
   * Devices are considered equal if
   *  vendor ID and product ID are the same.
   * Additionnaly, if one object contains
   *  a serial ID, they are compared, else not.
   */
  bool operator==(const mdtDeviceInfo &other);

  /*! \brief Comparaison operator
   *
   * Note that display text is not compared.
   */
  bool operator!=(const mdtDeviceInfo &other);

private:

  int pvVendorId;
  QString pvVendorName;
  int pvProductId;
  QString pvProductName;
  QString pvSerialId;
  int pvProtocolId;
  QString pvDisplayText;
};

Q_DECLARE_METATYPE(mdtDeviceInfo)

#endif  // #ifndef MDT_DEVICE_INFO_H
