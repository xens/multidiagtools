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
#ifndef MDT_TT_LOGICAL_TEST_CABLE_EDITOR_H
#define MDT_TT_LOGICAL_TEST_CABLE_EDITOR_H

///#include "mdtError.h"
#include "mdtSqlForm.h"
#include <QObject>
#include <QSqlDatabase>
#include <QVariant>
#include <QList>

class QWidget;

/*! \brief Class for logical test connection cable edition
 */
class mdtTtLogicalTestCableEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtLogicalTestCableEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup all tables
   */
  bool setupTables();

  /*! \brief Create a test connection cable
   */
  ///void createCable();

  /*! \brief Connect a test cable
   */
  ///void connectTestCable();

  /*! \brief Disconnect a test cable
   */
  ///void disconnectTestCable();

 private slots:

  /*! \brief Add a test node unit
   */
  void addTestNodeUnit();

  /*! \brief Remove test nod units
   */
  void removeTestNodeUnits();

  /*! \brief Add a DUT unit
   */
  void addDutUnit();

  /*! \brief Remove DUT units
   */
  void removeDutUnits();

  /*! \brief Add a link
   */
  void addLink();

  /*! \brief Edit a link
   */
  void editLink();

  /*! \brief Remove links
   */
  void removeLinks();

  /*! \brief Generate links
   */
  void generateLinks();


 private:

  /*! \brief Select vehicle that contains unit to test
   */
  QVariant selectDutVehicleId();

  /*! \brief Select unit to test
   */
  QVariant selectDutUnitId(const QVariant & vehicleId);

  /*! \brief Select test node to use
   */
  QVariant selectTestNode();

  /*! \brief Select test cable to use
   */
  QVariant selectTestCable();

  /*! \brief Select start connector
   */
//   QVariant selectStartConnectorId(const QVariant & dutUnitId);

  /*! \brief Select end connector(s) from given list
   */
//   QList<QVariant> selectEndConnectorIdList(const QList<QVariant> & unitConnectorIdList);

  /*! \brief Setup test cable table
   */
  bool setupTestCableTable();

  /*! \brief Setup test link table
   */
  bool setupTestLinkTable();

  /*! \brief Setup TestCable_TestNodeUnit_view
   */
  bool setupTestCableTestNodeUnitTable();

  /*! \brief Setup TestCable_DutUnit_view
   */
  bool setupTestCableDutUnitTable();

  Q_DISABLE_COPY(mdtTtLogicalTestCableEditor);

};

#endif // #ifndef MDT_TT_LOGICAL_TEST_CABLE_EDITOR_H
