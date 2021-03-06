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
#ifndef MDT_TT_TEST_MODEL_ITEM_H
#define MDT_TT_TEST_MODEL_ITEM_H

#include "mdtTtBase.h"
#include "mdtSqlTableSelection.h"
#include "mdtTtTestModelItemData.h"
#include "mdtTtTestNodeUnitSetupData.h"
#include "mdtTtTestModelItemRouteData.h"
#include <QVariant>
#include <QString>
#include <QModelIndex>
#include <QList>

/*! \brief Helper class for test item edition
 */
class mdtTtTestModelItem : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtTtTestModelItem(QObject *parent, QSqlDatabase db);

  /*! \brief Get SQL statement for test link selection.
   *
   * List all test links that not exists in TestModelItem_TestLink_tbl
   *  for given test model item ID, and that exist in TestModel_TestNode_tbl
   *  for given TestModel ID.
   */
  QString sqlForTestLinkSelection(const QVariant & testModelItemId, const QVariant & testModelId) const;

  /*! \brief Get SQL statement for test node unit selection
   *
   * List all test node units that not exists
   *  in TestNodeUnitSetup_tbl for given testModelItemId,
   *  and that exists in TestModel_TestNode_tbl for given testModelId.
   */
  QString sqlForTestNodeUnitSelection(const QVariant & testModelItemId, const QVariant & testModelId) const;

  /*! \brief Get SQL statement for test node selection
   */
  QString sqlForTestNodeSelection(const QVariant & testModelId) const;

  /*! \brief Get a list of used node unit IDs for given test item ID
   *
   * If given type is not null, only matching test node unit will be returned
   */
  QList<QVariant> getUsedNodeUnitIdList(const QVariant & testItemId, const QVariant & type);

  /*! \brief Get list of test node unit setup for given test item ID
   */
  QList<QVariant> getTestNodeUnitSetupIdList(const QVariant & testItemId);

  /*! \brief Add a test link to TestModelItem_TestLink_tbl
   */
  bool addTestLink(const QVariant & testModelItemId, const QVariant & testLinkId);

  /*! \brief Check if a test link exists in TestModelItem_TestLink_tbl
   */
  bool testLinkExists(const QVariant & testModelItemId, const QVariant & testLinkId, bool & ok);

  /*! \brief Remove test links
   */
  bool removeTestLinks(const mdtSqlTableSelection & s);

  /*! \brief Add a test node unit to TestNodeUnitSetup_tbl
   *
   * If given setup allready exists (a couple of PK: TestModelItem_Id_FK, TestNodeUnit_Id_FK)
   *  it will be updated with given data.
   */
  bool addOrUpdateTestNodeUnitSetup(const mdtTtTestNodeUnitSetupData & data);

  /*! \brief Add a list of test node unit to TestNodeUnitSetup_tbl
   */
  bool addOrUpdateTestNodeUnitSetupList(const QList<mdtTtTestNodeUnitSetupData> & dataList);

  /*! \brief Check if a node unit setup exists
   */
  bool testNodeUnitSetupExists(const QVariant & testModelItemId, const QVariant & testNodeUnitId, bool *ok);

  /*! \brief Get test node unit setup data
   */
  mdtTtTestNodeUnitSetupData getTestNodeUnitSetupData(const QVariant & testModelItemId, const QVariant & testNodeUnitId, bool *ok);

  /*! \brief Update test node unit setup data
   */
  bool updateTestNodeUnitData(const QVariant & testModelItemId, const QVariant & testNodeUnitId, const mdtTtTestNodeUnitSetupData & data);

  /*! \brief Remove test node units from TestNodeUnitSetup_tbl
   */
  bool removeTestNodeUnitSetups(const mdtSqlTableSelection & s);

  /*! \brief Add a test model item to TestModelItem_tbl
   *
   * Will also routes contained in data using addRoute(mdtTtTestModelItemRouteData, bool)
   */
  bool addItem(mdtTtTestModelItemData data, bool handleTransaction);

  /*! \brief Add a route to TestModelItemRoute_tbl
   *
   *  Will also add test link <-> test model item association
   *   and add or update needed test node unit setup (for path relays)
   */
  bool addRoute(mdtTtTestModelItemRouteData routeData, bool handleTransaction);

  /*! \brief Add a route to TestModelItemRoute_tbl
   *
   * \sa addRoute(mdtTtTestModelItemRouteData, bool)
   */
  bool addRoute(const QVariant & testModelItemId, const QVariant & testLinkId, const QVariant & measureTestNodeUnitConnectionId, const QList<mdtTtTestNodeUnitSetupData> & dataList, bool handleTransaction);

  /*! \brief Remove a route from TestModelItemRoute_tbl
   *
   * \pre s must not be empty
   * \pre s must contain field Id_PK with a non null value
   */
  bool removeRoutes(const mdtSqlTableSelection & s);

  /*! \brief Add a test node unit setup
   */
  bool addTestNodeUnitSetup(const QVariant & testItemId, const QVariant & testNodeUnitId, const QVariant & state);

  /*! \brief Remove test node unit setup
   */
  bool removeTestNodeUnitSetup(const QVariant & tnusId);

  /*! \brief Remove test node unit setup
   */
  ///bool removeTestNodeUnitSetups(const QModelIndexList & indexListOfSelectedRows);

  /*! \brief Remove all test node unit setup for given test item ID
   */
  bool removeTestNodeUnitSetupsForTestItemId(const QVariant & testItemId);

 private:

  Q_DISABLE_COPY(mdtTtTestModelItem);
};

#endif // #ifndef MDT_TT_TEST_MODEL_ITEM_H
