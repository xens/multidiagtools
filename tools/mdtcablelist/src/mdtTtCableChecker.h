/****************************************************************************
 **
 ** Copyright (C) 2011-2014 Philippe Steinmann.
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
#ifndef MDT_TT_CABLE_CHECKER_H
#define MDT_TT_CABLE_CHECKER_H

#include "mdtSqlForm.h"
#include <QSqlDatabase>
#include <QVariant>
#include <QString>
#include <QMap>
#include <QList>

class mdtTtTest;
class mdtTtTestResult;
class mdtDevice;
class mdtDeviceModbusWago;
class mdtDeviceU3606A;
class QObject;
class QWidget;
class QGridLayout;
namespace mdtTtCableCheckerPrivate
{
  struct deviceStatusWidget;
}

/*! \brief Cable checker class
 */
class mdtTtCableChecker : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtCableChecker(QWidget *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtTtCableChecker();

  /*! \brief Setup all tables
   */
  bool setupTables();

 private slots:

  /*! \brief Select the test model on witch this test is based
   */
  void setTestModel();

  /*! \brief Remove test result
   *
   * Will also remove all related items
   */
  void removeTestResult();

  /*! \brief Run complete test
   */
  void runTest();

 private:

  /*! \brief Select test model
   */
  QVariant selectTestModel();

  /*! \brief Setup test result table
   */
  bool setupTestTable();

  /*! \brief Setup test result item table
   */
  bool setupTestItemTable();

  /*! \brief Create multimeter
   */
  void createMultimeter();

  /*! \brief Remove multimeter
   */
  void removeMultimeter();

  /*! \brief Create I/O nodes
   */
  void createIoNodes();

  /*! \brief Remove I/O nodes
   */
  void removeIoNodes();

  /*! \brief Setup nodes
   */
  bool setupNodes(const QVariant & testItemId);

  /*! \brief Connect to instruments
   */
  bool connectToInstruments();

  /*! \brief Disconnect from instruments
   */
  void disconnectFromInstruments();

  /*! \brief Add device status widget
   */
  void addDeviceStatusWidget(mdtDevice *device, const QString & label);

  /*! \brief remove a device status widget
   */
  void removeDeviceStatusWidget(mdtDevice *device);

  Q_DISABLE_COPY(mdtTtCableChecker);

  mdtDeviceU3606A *pvMultimeter;
  QList<mdtDeviceModbusWago*> pvModbusIoNodes;
  // Device status widgets page
  QGridLayout *pvDeviceStatusWidgetsLayout;
  QMap<mdtDevice*, mdtTtCableCheckerPrivate::deviceStatusWidget*> pvDeviceStatusWidgets;
  // Test helper object
  mdtTtTest *pvTest;
};

#endif // #ifndef MDTTTCABLECHECKER_H
