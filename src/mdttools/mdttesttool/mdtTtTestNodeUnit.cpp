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
#include "mdtTtTestNodeUnit.h"
#include "mdtClUnit.h"
#include "mdtClPathGraph.h"
#include "mdtSqlRecord.h"
#include <QSqlRecord>

#include <QDebug>

mdtTtTestNodeUnit::mdtTtTestNodeUnit(QObject *parent, const QSqlDatabase & db)
 : mdtTtBase(parent, db)
{
}

QString mdtTtTestNodeUnit::sqlForTestConnectionSelection(const QList<QVariant> connectionIdList) const
{
  QString sql;
  int i;

  if(connectionIdList.isEmpty()){
    return sql;
  }
  sql = "SELECT * FROM UnitConnection_view ";
  Q_ASSERT(connectionIdList.size() > 0);
  sql += " WHERE UnitConnection_Id_PK = " + connectionIdList.at(0).toString();
  for(i = 1; i < connectionIdList.size(); ++i){
    sql += " OR UnitConnection_Id_PK = " + connectionIdList.at(i).toString();
  }

  return sql;
}

QList<QVariant> mdtTtTestNodeUnit::getConnectionIdListOfUnitId(const QVariant & unitId, bool *ok)
{
  Q_ASSERT(ok != 0);

  QList<QVariant> idList;
  QList<QSqlRecord> dataList;
  QString sql;
  int i;

  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + unitId.toString();
  dataList = mdtTtBase::getData(sql, ok);
  if(!*ok){
    return idList;
  }
  for(i = 0; i < dataList.size(); ++i){
    idList.append(dataList.at(i).value(0));
  }
  *ok = true;

  return idList;
}

QList<QVariant> mdtTtTestNodeUnit::getConnectionIdListLinkedToConnectionIdList(const QList<QVariant> connectionIdList, bool *ok)
{
  Q_ASSERT(ok != 0);

  QList<QVariant> idList;
  QList<QVariant> linkedConnectionIdList;
  mdtClPathGraph graph(database());
  int i;

  // Load link list
  if(!graph.loadLinkList()){
    pvLastError = graph.lastError();
    *ok = false;
    return idList;
  }
  // Add linked connection IDs for each given connection ID
  for(i = 0; i < connectionIdList.size(); ++i){
    Q_ASSERT(!connectionIdList.at(i).isNull());
    linkedConnectionIdList = graph.getLinkedConnectionIdList(connectionIdList.at(i)); /// \todo Have no error checking..
    idList.append(linkedConnectionIdList);
  }
  *ok = true;

  return idList;
}

mdtSqlRecord mdtTtTestNodeUnit::getUnitData(const QVariant & unitId, bool *ok)
{
  Q_ASSERT(ok != 0);

  QString sql;
  QList<QSqlRecord> dataList;
  mdtSqlRecord data;

  if(unitId.isNull()){
    pvLastError.setError("Trying to get unit data for a NULL unit ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeUnit");
    pvLastError.commit();
    *ok = false;
    return data;
  }
  sql = "SELECT * FROM Unit_tbl WHERE Id_PK = " + unitId.toString();
  dataList = mdtTtBase::getData(sql, ok);
  if(!*ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  *ok = true;

  return data;
}

mdtTtTestNodeUnitData mdtTtTestNodeUnit::getData(const QVariant & nodeUnitId, bool *ok, bool includeTestConnectionData) 
{
  Q_ASSERT(ok != 0);

  mdtClUnit unit(0, database());
  QString sql;
  mdtTtTestNodeUnitData data;
  QList<QSqlRecord> dataList;
  mdtSqlRecord unitData;

  if(nodeUnitId.isNull()){
    pvLastError.setError("Trying to get node unit data for a NULL node unit ID", mdtError::Warning);
    MDT_ERROR_SET_SRC(pvLastError, "mdtTtTestNodeUnit");
    pvLastError.commit();
    *ok = false;
    return data;
  }
  // Get TestNodeUnit_tbl part
  sql = "SELECT * FROM TestNodeUnit_tbl WHERE Unit_Id_FK_PK = " + nodeUnitId.toString();
  dataList = mdtTtBase::getData(sql, ok);
  if(!*ok){
    return data;
  }
  if(dataList.isEmpty()){
    return data;
  }
  Q_ASSERT(dataList.size() == 1);
  data = dataList.at(0);
  // Get Unit_tbl part
  unitData = getUnitData(nodeUnitId, ok);
  if(!*ok){
    data.clearValues();
    return data;
  }
  data.setUnitData(unitData);
  // Get test connection data, if required
  /**
  if(includeTestConnectionData){
    mdtClUnitConnectionData connectionData;
    connectionData = unit.getConnectionData(data.value("TestConnection_Id_FK"), false, ok);
    if(!*ok){
      return data;
    }
    data.setTestConnectionData(connectionData);
  }
  */
  // Done
  *ok = true;

  return data;
}

bool mdtTtTestNodeUnit::add(const mdtTtTestNodeUnitData & data) 
{
  if(!beginTransaction()){
    return false;
  }
  if(!addRecord(data, "TestNodeUnit_tbl")){
    rollbackTransaction();
    return false;
  }
  if(!addConnections(data.value("Unit_Id_FK_PK"), QVariant(), false)){
    rollbackTransaction();
    return false;
  }
  if(!commitTransaction()){
    return false;
  }

  return true;
}

bool mdtTtTestNodeUnit::edit(const QVariant & nodeUnitId, const mdtTtTestNodeUnitData & data) 
{
  return updateRecord("TestNodeUnit_tbl", data, "Unit_Id_FK_PK", nodeUnitId);
}

bool mdtTtTestNodeUnit::remove(const QVariant & nodeUnitId) 
{
}

bool mdtTtTestNodeUnit::addConnection(const QVariant & unitConnectionId, const QVariant & testNodeUnitId, const QVariant & testNodeBusId)
{
  mdtSqlRecord record;

  // Setup record
  if(!record.addAllFields("TestNodeUnitConnection_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  // set data
  record.setValue("UnitConnection_Id_FK_PK", unitConnectionId);
  record.setValue("TestNodeUnit_Id_FK", testNodeUnitId);
  record.setValue("TestNodeBus_Id_FK", testNodeBusId);

  // Add record
  return addRecord(record, "TestNodeUnitConnection_tbl");
}

bool mdtTtTestNodeUnit::addConnections(const QList<QVariant> & unitConnectionIdList, const QVariant & testNodeUnitId, const QVariant & testNodeBusId, bool handleTransaction)
{
  int i;

  if(handleTransaction){
    if(!beginTransaction()){
      return false;
    }
  }
  for(i = 0; i < unitConnectionIdList.size(); ++i){
    if(!addConnection(unitConnectionIdList.at(i), testNodeUnitId, testNodeBusId)){
      rollbackTransaction();
      return false;
    }
  }
  if(handleTransaction){
    if(!commitTransaction()){
      return false;
    }
  }

  return true;
}

bool mdtTtTestNodeUnit::addConnections(const QVariant & testNodeUnitId, const QVariant & testNodeBusId, bool handleTransaction)
{
  QString sql;
  QList<QSqlRecord> dataList;
  QList<QVariant> idList;
  bool ok;
  int i;

  // Get list of connections that are part of given unit ID (i.e. testNodeUnitId)
  sql = "SELECT Id_PK FROM UnitConnection_tbl WHERE Unit_Id_FK = " + testNodeUnitId.toString();
  dataList = mdtTtBase::getData(sql, &ok);
  if(!ok){
    return false;
  }
  for(i = 0; i < dataList.size(); ++i){
    idList.append(dataList.at(i).value(0));
  }

  // Add connections
  return addConnections(idList, testNodeUnitId, testNodeBusId, handleTransaction);
}

bool mdtTtTestNodeUnit::setBusIdToConnection(const QVariant & unitConnectionId, const QVariant & busId)
{
  QString sql;
  QList<QSqlRecord> dataList;
  mdtSqlRecord record;
  bool ok;

  // Get record of given unit connection ID
  sql = "SELECT * FROM TestNodeUnitConnection_tbl WHERE UnitConnection_Id_FK_PK = " + unitConnectionId.toString();
  dataList = mdtTtBase::getData(sql, &ok);
  if(!ok){
    return false;
  }
  Q_ASSERT(dataList.size() == 1);
  record = dataList.at(0);
  // Edit record
  record.setValue("TestNodeBus_Id_FK", busId);

  return updateRecord("TestNodeUnitConnection_tbl", record, "UnitConnection_Id_FK_PK", unitConnectionId);
}
