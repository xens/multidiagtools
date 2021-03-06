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
#ifndef MDT_TT_DATABASE_SCHEMA_H
#define MDT_TT_DATABASE_SCHEMA_H

#include "mdtSqlSchemaTable.h"
#include "mdtError.h"
#include <QList>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QSqlDatabase>

class mdtSqlDatabaseManager;

/*! \brief Build database schema
 */
class mdtTtDatabaseSchema
{
 public:

  /*! \brief Constructor
   */
  mdtTtDatabaseSchema(mdtSqlDatabaseManager *dbManager);

  /*! \brief Destructor
   */
  ~mdtTtDatabaseSchema();

  /*! \brief Create schema using Sqlite
   *
   * Ask the user to enter a path and create schema .
   * The startDirectory is used as directory to go when displaying the file dialog .
   */
  bool createSchemaSqlite(const QDir & startDirectory);

  /*! \brief Create schema using Sqlite
   *
   * Will overwrite database if exists .
   */
  bool createSchemaSqlite(const QFileInfo & dbFileInfo);

  /*! \brief Create schema using Sqlite
   *
   * \pre A database must allready be open .
   */
  bool createSchemaSqlite();

  /*! \brief Import a database
   *
   * Will ask the user to choose a source database and import it to currently open database .
   * The startDirectory is used as directory to go when displaying the file dialog .
   *
   * \pre A database (witch is destination database) must allready be open .
   */
  bool importDatabase(const QDir & startDirectory);

  /*! \brief Import a database
   *
   * Will import database given by sourceDbFileInfo into current open database .
   *
   * \pre A database (witch is destination database) must allready be open .
   */
  bool importDatabase(const QFileInfo sourceDbFileInfo);

  /*! \brief Check validity of a schema
   *
   * \todo Current version only checks that all expected tables exists .
   *
   * \pre A database must allready be open .
   */
  bool checkSchema();

  /*! \brief Get last error
   */
  mdtError lastError() const;

 private:

  /*! \brief
   */
  bool setupTables();

  /*! \brief
   */
  bool createTablesSqlite();

  /*! \brief Create triggers
   */
  bool createTriggers();

  /*! \brief
   */
  bool createViews();

  /*! \brief Populate database with base data
   */
  bool populateTables();

  /*! \brief
   */
  bool setupVehicleTypeTable();

  /*! \brief
   */
  bool setupVehicleTypeUnitTable();

  /*! \brief
   */
  bool setupVehicleTypeLinkTable();

  /*! \brief Setup connection type table
   */
  bool setupConnectionTypeTable();

  /*! \brief Setup connector table
   */
  bool setupConnectorTable();

  /*! \brief Setup Connector contact table
   */
  bool setupConnectorContactTable();

  /*! \brief
   */
  bool setupArticleTable();

  /*! \brief
   */
  bool setupArticleComponentTable();

  /*! \brief Setup article connector table
   */
  bool setupArticleConnectorTable();

  /*! \brief
   */
  bool setupArticleConnectionTable();

  /*! \brief
   */
  bool setupArticleLinkTable();

  /*! \brief
   */
  bool setupUnitTable();

  /*! \brief Setup unit connector table
   */
  bool setupUnitConnectorTable();

  /*! \brief
   */
  bool setupUnitConnectionTable();

  /*! \brief Create Wire_tbl
   *
   * Wire_tbl is a sort of wires databse
   */
  bool setupWireTable();

  /*! \brief Setup Modification_tbl
   */
  bool setupModificationTable();

  /*! \brief Setup LinkModification_tbl
   */
//   bool setupLinkModificationTable();

  /*! \brief Setup LinkVersion_tbl
   */
  bool setupLinkVersionTable();

  /*! \brief Create onLinkVersionAfterInsert trigger
   */
  bool createOnLinkVersionAfterInsertTrigger();

  /*! \brief Create Link_tbl
   */
  bool setupLinkTable();

  /*! \brief Setup LinkBeam_tbl
   */
  bool setupLinkBeamTable();

  /*! \brief Setup LinkBeam_Unit_tbl
   */
  bool setupLinkBeamUnitStartTable();

  /*! \brief Setup LinkBeam_Unit_tbl
   */
  bool setupLinkBeamUnitEndTable();

  /*! \brief
   */
  bool setupLinkDirectionTable();

  /*! \brief Setup TestSystem_tbl
   */
  bool setupTestSystemTable();

  /*! \brief Setup TestSystemComponentType_tbl
   */
  bool setupTestSystemComponentTypeTable();

  /*! \brief Setup TestSystemComponent_tbl
   */
  bool setupTestSystemComponentTable();

  /*! \brief Setup TestSystem_TestSystemComponent_tbl
   */
  bool setupTestSystem_TestSystemComponentTable();

  /*! \brief Setup TestSystemUnit_tbl
   */
  bool setupTestSystemUnitTable();

  /*! \brief Setup TestSystemUnitType_tbl
   */
  bool setupTestSystemUnitTypeTable();



  /*! \brief Setup TestNode_tbl
   *
   * \deprecated Will be replaced by NOTE
   */
  bool setupTestNodeTable();

  /*! \brief Setup TestNodeUnit_tbl
   */
  bool setupTestNodeUnitTable();

  /*! \brief Setup TestNodeUnitConnection_tbl
   */
  bool setupTestNodeUnitConnectionTable();

  /*! \brief Setup TestNodeRoute_tbl
   */
  bool setupTestNodeRouteTable();

  /*! \brief Setup TestNodeRouteUnit_tbl
   */
  bool setupTestNodeRouteUnitTable();

  /*! \brief Setup TestNodeUnitType_tbl
   */
  bool setupTestNodeUnitTypeTable();

  /*! \brief Setup TestNodeBus_tbl
   */
//   bool setupTestNodeBusTable();

  /*! \brief Setup TestCable_TestNodeUnit_tbl
   */
  bool setupTestCableTestNodeUnitTable();

  /*! \brief Setup TestCable_DutUnit_tbl
   */
  bool setupTestCableDutUnitTable();

  /*! \brief Setup TestCable_tbl
   */
  bool setupTestCableTable();

  /*! \brief Setup LogicalTestCable_tbl
   */
  bool setupLogicalTestCableTable();

  /*! \brief Setup TestLink_tbl
   */
  bool setupTestLinkTable();

  /*! \brief Setup TestModel_tbl
   */
  bool setupTestModelTable();

  /*! \brief Setup TestModel_TestNode_tbl
   */
  bool setupTestModelTestNodeTable();

  /*! \brief Setup test item table
   *
   * \todo Move to test tool when created
   */
  bool setupTestModelItemTable();

  /*! \brief Setup TestModelItem_TestLink_tbl
   */
  bool setupTestModelItemTestLinkTable();

  /*! \brief Setup test node unit setup table
   *
   * \todo Move to test tool when created
   */
  bool setupTestNodeUnitSetupTable();

  /*! \brief Setup TestModelItemRoute_tbl
   *
   * This table will contain routes (paths)
   *  from a test link to node units.
   */
//   bool setupTestModelItemRouteTable();

  /*! \brief Setup test result table
   *
   * \todo Move to test tool when created
   */
  bool setupTestTable();

  /*! \brief Setup test result item table
   *
   * \todo Move to test tool when created
   */
  bool setupTestItemTable();

  /*! \brief
   */
  bool setupLinkTypeTable();

  /*! \brief Create a trigger
   *
   * Will also drop trigger first if extits
   */
  bool createTrigger(const QString & triggerName, const QString & sql);

  /*! \brief Create a view
   *
   * Will also drop it first if extits
   */
  bool createView(const QString & viewName, const QString & sql);

  /*! \brief
   */
  bool createVehicleTypeUnitView();

  /*! \brief
   */
  bool createArticleComponentUsageView();

  /*! \brief
   */
  bool createArticleComponentView();

  /*! \brief Create article connection view
   */
  bool createArticleConnectorView();

  /*! \brief Create article connection view
   */
  bool createArticleConnectionView();

  /*! \brief
   */
  bool createArticleLinkView();

  /*! \brief
   */
  bool createUnitView();

  /*! \brief
   */
  bool createUnitComponentView();

  /*! \brief Create UnitConnector_view
   */
  bool createUnitConnectorView();

  /*! \brief Create UnitConnectorUsage_view
   *
   * Display wich units uses a given connector
   */
  bool createUnitConnectorUsageView();

  /*! \brief
   */
  bool createUnitConnectionView();

  /*! \brief Create a view that joins article link table with unit connections
   *
   * When creating a article, witch is a model that can be re-used in units,
   *  its possible to add links to this article. These links are added to ArticleLink_tbl.
   *
   * To find a path in the link table, article links must be followed.
   *  This can be done by:
   *   - Finding witch unit connection is linked to a article connection
   *   - Finding links between these article connections in article link table
   *   - Include the result in link list
   *
   * This view will give article links that are used by a unit connection .
   */
  bool createArticleLinkUnitConnectionView();

  /*! \brief
   */
  bool createUnitLinkView();

  /*! \brief
   */
  bool createUnitVehicleTypeView();

  /*! \brief
   */
  bool createLinkListView();

  /*! \brief Create LinkBeam_UnitStart_view
   */
  bool createLinkBeamUnitStartView();

  /*! \brief Create LinkBeam_UnitEnd_view
   */
  bool createLinkBeamUnitEndView();

  
  /*! \brief Create TestSystemComponent_view
   *
   * Lists components of a test system.
   */
  bool createTestSystemComponentView();

  /*! \brief Create TestSystemOfComponent_view
   *
   * Lists test systems assigned to a component
   */
  bool createTestSytemOfComponentView();

  /*! \brief Create TestSystemUnit_view
   */
  bool createTestSystemUnitView();


  /*! \brief Create TestNode_view
   */
  bool createTestNodeView();

  /*! \brief Create Unit_TestNode_view
   *
   * Display test nodes that are related to given unit
   */
  bool createUnitTestNodeView();

  /*! \brief Create TestNodeUnitConnection_view
   */
  bool createTestNodeUnitView();

  /*! \brief Create TestNodeUnitConnection_view
   */
  bool createTestNodeUnitConnectionView();

  /*! \brief Create test link view
   */
  bool createTestLinkView();

  /*! \brief Create TestCable_TestNodeUnit_view
   */
  bool createTestCableTestNodeUnitView();

  /*! \brief Create TestNode_TestCable_view
   */
  bool createTestNodeTestCableView();

  /*! \brief Create TestCable_DutUnit_view
   */
  bool createTestCableDutUnitView();

  /*! \brief Create TestModel_TestNode_view
   */
  bool createTestModelTestNodeView();

  /*! \brief Create TestModelItem_TestLink_view
   */
  bool createTestModelItemTestLinkView();

  /*! \brief Create TestNodeUnitSetup_view
   */
  bool createTestNodeUnitSetupView();

  /*! \brief Create TestNodeRoute_view
   */
  bool createTestNodeRouteView();

  /*! \brief Create TestModelItemRoute_view
   */
  bool createTestModelItemRouteView();

  /*! \brief Create test item link view
   *
   * \todo Move to test tool when created
   */
  ///bool createTestModelItemView();

  /*! \brief Create test model item node unit view
   *
   * \todo Move to test tool when created
   */
  ///bool createTestModelItemNodeUnitSetupView();

  /*! \brief Create view to get nodes used by a test item
   *
   * \todo Move to test tool when created
   */
  ///bool createTestModelItemNodeView();

  /*! \brief Create Test_view
   */
  bool createTestView();

  /*! \brief Create view to get test result item
   */
  bool createTestItemView();

  /*! \brief Create test item node unit view
   *
   * \todo Move to test tool when created
   */
  bool createTestItemNodeUnitSetupView();

  /*! \brief Create view to get node units used by a test item
   *
   * \todo Move to test tool when created
   */
  ///bool createTestModelItemNodeUnitView();

  /*! \brief Check if given PK data exists in given table name
   */
  bool pkExistsInTable(const QString & tableName, const QString & pkField, const QVariant & pkData);

  /*! \brief Insert data into a table
   *
   * Note: first field must be primary key
   */
  bool insertDataIntoTable(const QString & tableName, const QStringList & fields, const QList<QVariant> & data);

  /*! \brief Get SQL statement for data insertion
   */
  QString sqlForDataInsertion(const QString & tableName, const QStringList & fields, const QList<QVariant> & data);

  /*! \brief Get SQL statement for data edition
   */
  QString sqlForDataEdition(const QString & tableName, const QStringList & fields, const QList<QVariant> & data);

  /*! \brief Populate Modification_tbl
   */
  bool populateModificationTable();

  /*! \brief Populate Connection type table
   */
  bool populateConnectionTypeTable();

  /*! \brief Populate LinkType table
   */
  bool populateLinkTypeTable();

  /*! \brief Populate LinkType table
   */
  bool populateLinkDirectionTable();

  /*! \brief Populate TestSystemComponentType_tbl
   */
  bool populateTestSystemComponentTypeTable();

  /*! \brief Populate TestSystemUnitType_tbl
   */
  bool populateTestSystemUnitTypeTable();


  /*! \brief Populate TestNodeUnitType table
   *
   * \todo Move to test tool when created
   */
  bool populateTestNodeUnitTypeTable();

  Q_DISABLE_COPY(mdtTtDatabaseSchema);

  mdtSqlDatabaseManager *pvDatabaseManager;
  QList<mdtSqlSchemaTable> pvTables;
  mdtError pvLastError;
};

#endif // #ifndef MDT_TT_DATABASE_SCHEMA_H
