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
#include "mdtClPathGraph.h"
#include "mdtClPathGraphicsConnection.h"
#include "mdtClPathGraphicsLink.h"
#include "mdtClUnit.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QString>
#include <QModelIndex>
#include <QObject>

//#include <QDebug>

using namespace mdtClPathGraphPrivate;

/*
 * Visitor class implementation
 */

mdtClPathGraphVisitor::mdtClPathGraphVisitor()
{
  pvVisitedEdgeList = 0;
}

mdtClPathGraphVisitor::mdtClPathGraphVisitor(QVector<edge_t> *edgeList)
{
  Q_ASSERT(edgeList != 0);
  pvVisitedEdgeList = edgeList;
}

void mdtClPathGraphVisitor::examine_edge(edge_t e, const graph_t & g)
{
  Q_ASSERT(pvVisitedEdgeList != 0);
  pvVisitedEdgeList->append(e);
}


/*
 * mdtClPathGraph implementation
 */

mdtClPathGraph::mdtClPathGraph(QSqlDatabase db)
{
  pvDatabase = db;
  pvLinkListModel = new QSqlQueryModel;
  pvGraphicsScene = new QGraphicsScene;
}

mdtClPathGraph::~mdtClPathGraph()
{
  delete pvLinkListModel;
  delete pvGraphicsScene;
}

bool mdtClPathGraph::loadLinkList() 
{
  QString sql;
  QSqlError sqlError;
  int row;
  QModelIndex index;
  QVariant data;
  QVariant startConnectionId, endConnectionId;
  bool isBidirectional;

  // Clear previous results
  pvGraph.clear();
  pvGraphVertices.clear();
  pvEdgeDataMap.clear();
  pvManuallyAddedEdges.clear();
  // Run query to get start and end connection IDs
  sql = "SELECT UnitConnectionStart_Id_FK, UnitConnectionEnd_Id_FK, LinkDirection_Code_FK FROM LinkList_view";
  pvLinkListModel->setQuery(sql, pvDatabase);
  sqlError = pvLinkListModel->lastError();
  if(sqlError.isValid()){
    pvLastError.setError(QObject::tr("Cannot load link list from database."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    return false;
  }
  // Build the graph
  for(row = 0; row < pvLinkListModel->rowCount(); ++row){
    // Get start connection ID
    index = pvLinkListModel->index(row, 0);
    data = pvLinkListModel->data(index);
    if(data.isNull()){
      pvLastError.setError(QObject::tr("Cannot load link list from database."), mdtError::Error);
      pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
      pvLastError.setSystemError(-1, QObject::tr("UnitConnectionStart_Id_FK returned a NULL value."));
      MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
      pvLastError.commit();
      return false;
    }
    startConnectionId = data;
    // Get end connection ID
    index = pvLinkListModel->index(row, 1);
    data = pvLinkListModel->data(index);
    if(data.isNull()){
      pvLastError.setError(QObject::tr("Cannot load link list from database."), mdtError::Error);
      pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
      pvLastError.setSystemError(-1, QObject::tr("UnitConnectionEnd_Id_FK returned a NULL value."));
      MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
      pvLastError.commit();
      return false;
    }
    endConnectionId = data;
    // Check if link is bidirectional
    index = pvLinkListModel->index(row, 2);
    data = pvLinkListModel->data(index);
    if(data.toString() == "BID"){
      isBidirectional = true;
    }else{
      isBidirectional = false;
    }
    // Add link
    addLinkPv(startConnectionId, endConnectionId, isBidirectional, 1, false, QVariant());
    // Fetch more data if possible
    if((row > 0)&&(pvLinkListModel->canFetchMore())){
      pvLinkListModel->fetchMore();
    }
  }

  return true;
}

/**
void mdtClPathGraph::addLink(const QVariant& startConnectionId, const QVariant& endConnectionId, bool isBidirectional, int weight)
{
  Q_ASSERT(!startConnectionId.isNull());
  Q_ASSERT(!endConnectionId.isNull());

  addLinkPv(startConnectionId, endConnectionId, isBidirectional, weight, true, QVariant());
}
*/

void mdtClPathGraph::addLink(const QVariant & startConnectionId, const QVariant & endConnectionId, const QVariant & userData, bool isBidirectional, int weight)
{
  Q_ASSERT(!startConnectionId.isNull());
  Q_ASSERT(!endConnectionId.isNull());

  addLinkPv(startConnectionId, endConnectionId, isBidirectional, weight, true, userData);
}

QVariant mdtClPathGraph::getUserData(const QVariant & startConnectionId, const QVariant & endConnectionId)
{
  Q_ASSERT(!startConnectionId.isNull());
  Q_ASSERT(!endConnectionId.isNull());

  vertex_t u, v;
  std::pair<edge_t, bool> ep;

  // Check if we have requested connection IDs in the graph
  if(!pvGraphVertices.contains(startConnectionId.toInt())){
    pvLastError.setError(QObject::tr("Cannot get user data."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(-1, QObject::tr("Start connection ID not found") + " (ID: " + startConnectionId.toString() + ").");
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    return QVariant();
  }
  if(!pvGraphVertices.contains(endConnectionId.toInt())){
    pvLastError.setError(QObject::tr("Cannot get user data."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(-1, QObject::tr("End connection ID not found") + " (ID: " + endConnectionId.toString() + ").");
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    return QVariant();
  }
  // Get vertices, edge, and return data
  u = pvGraphVertices.value(startConnectionId.toInt());
  v = pvGraphVertices.value(endConnectionId.toInt());
  ep = boost::edge(u, v, pvGraph);
  if(!ep.second){
    return QVariant();
  }

  return pvEdgeDataMap.at(ep.first).userData;
}

void mdtClPathGraph::removeAddedLinks()
{
  int i;

  for(i = 0; i < pvManuallyAddedEdges.size(); ++i){
    boost::remove_edge(pvManuallyAddedEdges.at(i), pvGraph);
  }
}

QList<QVariant> mdtClPathGraph::getLinkedConnectionIdList(const QVariant & fromConnectionId, bool & ok)
{
  QList<QVariant> connectionIdList;
  mdtClPathGraphEdgeData edgeData;
  QVector<edge_t> visitedEdgeList;
  mdtClPathGraphVisitor visitor(&visitedEdgeList);
  vertex_t vertex;
  int i;

  // Check if we have requested connection ID in the graph
  if(!pvGraphVertices.contains(fromConnectionId.toInt())){
    ok = false;
    pvLastError.setError(QObject::tr("Cannot get list of linked connections."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(-1, QObject::tr("Start connection ID not found") + " (ID: " + fromConnectionId.toString() + ").");
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    return connectionIdList;
  }
  vertex = pvGraphVertices.value(fromConnectionId.toInt());
  // Proceed BFS
  breadth_first_search(pvGraph, vertex, boost::visitor(visitor));
  // Get connections
  for(i = 0; i < visitedEdgeList.size(); ++i){
    edgeData = pvEdgeDataMap.at(visitedEdgeList.at(i));
    Q_ASSERT(!edgeData.startConnectionId.isNull());
    Q_ASSERT(!edgeData.endConnectionId.isNull());
    if(!edgeData.isComplement){
      if((edgeData.startConnectionId != fromConnectionId)&&(!connectionIdList.contains(edgeData.startConnectionId))){
        connectionIdList.append(edgeData.startConnectionId);
      }
      if((edgeData.endConnectionId != fromConnectionId)&&(!connectionIdList.contains(edgeData.endConnectionId))){
        connectionIdList.append(edgeData.endConnectionId);
      }
    }
  }
  ok = true;

  return connectionIdList;
}

QList<QVariant> mdtClPathGraph::getLinkedConnectorIdList(const QVariant & fromConnectorId, bool *ok, const QList<QVariant> & connectionIdListToIgnore)
{
  Q_ASSERT(ok != 0);

  mdtClUnit unit(0, pvDatabase);
  QVariant connectorId;
  QList<QVariant> connectorIdList;
  QList<QVariant> fromConnectionIdList;
  QList<QVariant> linkedConnectionIdList;
  int i;
  ///int k;

  // Get fromConnector's list of connections
  fromConnectionIdList = unit.getConnectionIdListPartOfConnectorId(fromConnectorId, ok);
  if(!*ok){
    pvLastError = unit.lastError();
    return connectorIdList;
  }
  // Remove connectionIdListToIgnore in fromConnectionIdList
  for(i = 0; i < fromConnectionIdList.size(); ++i){
    if(connectionIdListToIgnore.contains(fromConnectionIdList.at(i))){
      fromConnectionIdList.removeAt(i);
    }
  }
  // Add linked connectors for each connection
  for(auto & fromConnectionId : fromConnectionIdList){
    // Get linked connections
    if(connectionExists(fromConnectionId)){
      linkedConnectionIdList = getLinkedConnectionIdList(fromConnectionId, *ok);
      if(!*ok){
        connectorIdList.clear();
        return connectorIdList;
      }
      // Buil connectors list that contains linked connections
      for(auto & linkedConnectionId : linkedConnectionIdList){
        if(connectionExists(linkedConnectionId)){
          // Get connector ID
          connectorId = unit.getConnectorIdOfConnectionId(linkedConnectionId, ok);
          if(!*ok){
            pvLastError = unit.lastError();
            return connectorIdList;
          }
          // Add connector ID to result
          if((!connectorId.isNull())&&(!connectorIdList.contains(connectorId))){
            connectorIdList.append(connectorId);
          }
        }
      }
    }
  }
//   for(i = 0; i < fromConnectionIdList.size(); ++i){
//     // Get linked connections
//     linkedConnectionIdList = getLinkedConnectionIdList(fromConnectionIdList.at(i), *ok);
// //     if(!*ok){
// //       connectorIdList.clear();
// //       return connectorIdList;
// //     }
//     for(k = 0; k < linkedConnectionIdList.size(); ++k){
//       // Get connector ID
//       connectorId = unit.getConnectorIdOfConnectionId(linkedConnectionIdList.at(k), ok);
//       if(!*ok){
//         pvLastError = unit.lastError();
//         return connectorIdList;
//       }
//       // Add connector ID to result
//       if((!connectorId.isNull())&&(!connectorIdList.contains(connectorId))){
//         connectorIdList.append(connectorId);
//       }
//     }
//   }
  *ok = true;

  return connectorIdList;
}

QList<QVariant> mdtClPathGraph::getShortestPath(const QVariant & fromConnectionId, const QVariant & toConnectionId, bool & ok)
{
  QList<QVariant> connectionIdList;
  mdtClPathGraphEdgeData edgeData;
  std::vector<vertex_t> predecessors(boost::num_vertices(pvGraph)); // To store parents
  std::vector<int> distances(boost::num_vertices(pvGraph));         // To store distances
  typedef boost::property_map<graph_t, boost::vertex_index_t>::type index_map_t;
  index_map_t indexMap = boost::get(boost::vertex_index, pvGraph);
  boost::iterator_property_map<vertex_t*, index_map_t, vertex_t, vertex_t&> predecessorMap(&predecessors[0], indexMap);
  boost::iterator_property_map<int*, index_map_t, int, int&> distanceMap(&distances[0], indexMap);
  vertex_t fromVertex, toVertex;

  // Check if we have requested from connection ID in the graph
  if(!pvGraphVertices.contains(fromConnectionId.toInt())){
    ok = false;
    pvLastError.setError(QObject::tr("Cannot get shortest path."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(-1, QObject::tr("Source connection ID not found") + " (ID: " + fromConnectionId.toString() + ").");
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    return connectionIdList;
  }
  fromVertex = pvGraphVertices.value(fromConnectionId.toInt());
  // Check if we have requested to connection ID in the graph
  if(!pvGraphVertices.contains(toConnectionId.toInt())){
    ok = false;
    pvLastError.setError(QObject::tr("Cannot get shortest path."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(-1, QObject::tr("Destination connection ID not found") + " (ID: " + toConnectionId.toString() + ").");
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    return connectionIdList;
  }
  toVertex = pvGraphVertices.value(toConnectionId.toInt());
  // Proceed Dijkstra shortest path search
  boost::dijkstra_shortest_paths(pvGraph, fromVertex, boost::distance_map(distanceMap).predecessor_map(predecessorMap));
  // Extract the requested shortest path itself
  vertex_t v = toVertex;  // We start at the destination and work our way back to the source
  for(vertex_t u = predecessorMap[v]; u != v; v = u, u = predecessorMap[v]){
    std::pair<edge_t, bool> ep = boost::edge(u, v, pvGraph);
    Q_ASSERT(ep.second);
    edgeData = pvEdgeDataMap.at(ep.first);
    Q_ASSERT(!edgeData.startConnectionId.isNull());
    Q_ASSERT(!edgeData.endConnectionId.isNull());
    if(edgeData.isComplement){
      if(!connectionIdList.contains(edgeData.startConnectionId)){
        connectionIdList.prepend(edgeData.startConnectionId);
      }
      if(!connectionIdList.contains(edgeData.endConnectionId)){
        connectionIdList.prepend(edgeData.endConnectionId);
      }
    }else{
      if(!connectionIdList.contains(edgeData.endConnectionId)){
        connectionIdList.prepend(edgeData.endConnectionId);
      }
      if(!connectionIdList.contains(edgeData.startConnectionId)){
        connectionIdList.prepend(edgeData.startConnectionId);
      }
    }
  }
  ok = true;

  return connectionIdList;
}

bool mdtClPathGraph::connectionsAreLinked(const QVariant & connectionIdA, const QVariant & connectionIdB)
{
  Q_ASSERT(!connectionIdA.isNull());
  Q_ASSERT(!connectionIdB.isNull());

  bool ok;
  bool areLinked;

  if(!pvGraphVertices.contains(connectionIdA.toInt())){
    return false;
  }
  if(!pvGraphVertices.contains(connectionIdB.toInt())){
    return false;
  }
  areLinked = !getShortestPath(connectionIdA, connectionIdB, ok).isEmpty();
  Q_ASSERT(ok);

  return areLinked;
}

bool mdtClPathGraph::drawPath(const QVariant & fromConnectionId)
{
  mdtClPathGraphEdgeData edgeData;
  mdtClPathGraphicsConnection *startConnection, *endConnection;
  mdtClPathGraphicsLink *link;
  int startConnectionId, endConnectionId;
  QVector<edge_t> visitedEdgeList;
  mdtClPathGraphVisitor visitor(&visitedEdgeList);
  vertex_t vertex;
  int i;

  // Check if we have requested connection ID in the graph
  if(!pvGraphVertices.contains(fromConnectionId.toInt())){
    pvLastError.setError(QObject::tr("Cannot draw requested path."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(-1, QObject::tr("Start connection ID not found") + " (ID: " + fromConnectionId.toString() + ").");
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    return false;
  }
  vertex = pvGraphVertices.value(fromConnectionId.toInt());
  // Clear previous results
  pvDrawnConnections.clear();
  pvGraphicsScene->clear();
  // Proceed BFS
  breadth_first_search(pvGraph, vertex, boost::visitor(visitor));
  // draw ...
  for(i = 0; i < visitedEdgeList.size(); ++i){
    edgeData = pvEdgeDataMap.at(visitedEdgeList.at(i));
    Q_ASSERT(!edgeData.startConnectionId.isNull());
    Q_ASSERT(!edgeData.endConnectionId.isNull());
    if(!edgeData.isComplement){
      // Try to get startConnection
      startConnectionId = edgeData.startConnectionId.toInt();
      startConnection = pvDrawnConnections.value(startConnectionId, 0);
      // Try to get end connection
      endConnectionId = edgeData.endConnectionId.toInt();
      endConnection = pvDrawnConnections.value(endConnectionId, 0);
      // Create missing connections
      if((startConnection == 0)&&(endConnection == 0)){
        startConnection = newConnectionItem(startConnectionId, 0, false);
        endConnection = newConnectionItem(endConnectionId, startConnection, false);
      }else if(startConnection == 0){
        Q_ASSERT(endConnection != 0);
        startConnection = newConnectionItem(startConnectionId, endConnection, false);
      }else{
        Q_ASSERT(startConnection != 0);
        endConnection = newConnectionItem(endConnectionId, startConnection, false);
      }
      Q_ASSERT(startConnection != 0);
      Q_ASSERT(endConnection != 0);
      // Draw link
      link = new mdtClPathGraphicsLink(startConnection, endConnection);
      pvGraphicsScene->addItem(link);
      // Set connections and link data
      setGraphicsItemsData(startConnection, endConnection, link, startConnectionId, endConnectionId);
    }
  }

  return true;
}

void mdtClPathGraph::attachView(QGraphicsView *view) 
{
  Q_ASSERT(view != 0);

  view->setScene(pvGraphicsScene);
}

/**
QStringList mdtClPathGraph::lastErrorMessage() const
{
  QStringList errorList;

  errorList << pvLastError.text() << pvLastError.informativeText() << pvLastError.systemText();
  Q_ASSERT(errorList.size() == 3);
  return errorList;
}
*/

mdtError mdtClPathGraph::lastError() const
{
  return pvLastError;
}

void mdtClPathGraph::addLinkPv(const QVariant & startConnectionId, const QVariant & endConnectionId, bool isBidirectional, int weight, bool addToManuallyList, const QVariant & userData)
{
  Q_ASSERT(!startConnectionId.isNull());
  Q_ASSERT(!endConnectionId.isNull());

  mdtClPathGraphEdgeData edgeData;
  std::pair<edge_t, bool> ep;       // To get edge added with add_edge()
  vertex_t startVertex, endVertex;
  int _startConnectionId;
  int _endConnectionId;

  // Get start vertex or create one if not allready exists
  _startConnectionId = startConnectionId.toInt();
  if(pvGraphVertices.contains(_startConnectionId)){
    startVertex = pvGraphVertices.value(_startConnectionId);
  }else{
    startVertex = boost::add_vertex(pvGraph);
    pvGraphVertices.insert(_startConnectionId, startVertex);
  }
  // Get end vertex or create one if not allready exists
  _endConnectionId = endConnectionId.toInt();
  if(pvGraphVertices.contains(_endConnectionId)){
    endVertex = pvGraphVertices.value(_endConnectionId);
  }else{
    endVertex = boost::add_vertex(pvGraph);
    pvGraphVertices.insert(_endConnectionId, endVertex);
  }
  // Setup edge data and add edge
  edgeData.startConnectionId = startConnectionId;
  edgeData.endConnectionId = endConnectionId;
  edgeData.isComplement = false;
  edgeData.userData = userData;
  ep = boost::add_edge(startVertex, endVertex, weight, pvGraph);
  Q_ASSERT(ep.second);
  pvEdgeDataMap.insert(std::pair<edge_t, mdtClPathGraphEdgeData>(ep.first, edgeData));
  if(addToManuallyList){
    pvManuallyAddedEdges.append(ep.first);
  }
  // Add complement edge if link is bidirectinnal
  if(isBidirectional){
    edgeData.isComplement = true;
    ep = boost::add_edge(endVertex, startVertex, weight, pvGraph);
    Q_ASSERT(ep.second);
    pvEdgeDataMap.insert(std::pair<edge_t, mdtClPathGraphEdgeData>(ep.first, edgeData));
    if(addToManuallyList){
      pvManuallyAddedEdges.append(ep.first);
    }
  }
}

mdtClPathGraphicsConnection *mdtClPathGraph::newConnectionItem(int id, mdtClPathGraphicsConnection *itemForNextPos, bool reverse)
{
  mdtClPathGraphicsConnection *item;

  item = new mdtClPathGraphicsConnection;
  if(itemForNextPos != 0){
    item->setPos(itemForNextPos->nextPos(reverse));
  }else{
    item->setPos(0.0, 0.0);
  }
  pvGraphicsScene->addItem(item);
  pvDrawnConnections.insert(id, item);

  return item;
}

bool mdtClPathGraph::setGraphicsItemsData(mdtClPathGraphicsConnection *startConnection, mdtClPathGraphicsConnection *endConnection, mdtClPathGraphicsLink *link, int startConnectionId, int endConnectionId)
{
  Q_ASSERT(startConnection != 0);
  Q_ASSERT(endConnection != 0);
  Q_ASSERT(link != 0);

  QString sql;
  QSqlError sqlError; 
  QVariant data;
  QSqlQuery query(pvDatabase);
  QSqlRecord record;
  QString str;
  QString strTmp;

  sql = "SELECT StartVehicleType, StartVehicleSubType, StartCabinet, StartSchemaPosition, StartAlias, StartUnitConnectorName, StartUnitContactName, StartConnectionType_Code_FK, "\
        "EndVehicleType, EndVehicleSubType, EndCabinet, EndSchemaPosition, EndAlias, EndUnitConnectorName, EndUnitContactName, EndConnectionType_Code_FK, Identification, "\
        "LinkType_Code_FK "\
        "FROM LinkList_view ";
  sql += " WHERE UnitConnectionStart_Id_FK = " + QString::number(startConnectionId);
  sql += " AND UnitConnectionEnd_Id_FK = " + QString::number(endConnectionId);
  if(!query.exec(sql)){
    sqlError = query.lastError();
    pvLastError.setError(QObject::tr("Cannot get connections informations from database."), mdtError::Error);
    pvLastError.setInformativeText(QObject::tr("Plese see details for more informations."));
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClPathGraph");
    pvLastError.commit();
    startConnection->setText("??");
    endConnection->setText("??");
    return false;
  }
  query.next();
  record = query.record();
  // Set start data
  str = record.value("StartVehicleType").toString() + "\n";
  str += record.value("StartVehicleSubType").toString() + "\n";
  strTmp = record.value("StartCabinet").toString().trimmed();
  if(!strTmp.isEmpty()){
    str += strTmp + "\n";
  }
  strTmp = record.value("StartSchemaPosition").toString().trimmed();
  if(!strTmp.isEmpty()){
    str += strTmp + "\n";
  }
  strTmp = record.value("StartAlias").toString().trimmed();
  if(!strTmp.isEmpty()){
    str += strTmp + "\n";
  }
  str += record.value("StartUnitConnectorName").toString() + ";";
  str += record.value("StartUnitContactName").toString();
  startConnection->setText(str);
  // Set start connection type
  str = record.value("StartConnectionType_Code_FK").toString();
  if(str == "P"){
    startConnection->setConnectionType(mdtClPathGraphicsConnection::Pin);
  }else if(str == "S"){
    startConnection->setConnectionType(mdtClPathGraphicsConnection::Socket);
  }else{
    startConnection->setConnectionType(mdtClPathGraphicsConnection::Terminal);
  }
  // Set end data
  str = record.value("EndVehicleType").toString() + "\n";
  str += record.value("EndVehicleSubType").toString() + "\n";
  strTmp = record.value("EndCabinet").toString().trimmed();
  if(!strTmp.isEmpty()){
    str += strTmp + "\n";
  }
  strTmp = record.value("EndSchemaPosition").toString().trimmed();
  if(!strTmp.isEmpty()){
    str += strTmp + "\n";
  }
  strTmp = record.value("EndAlias").toString().trimmed();
  if(!strTmp.isEmpty()){
    str += strTmp + "\n";
  }
  str += record.value("EndUnitConnectorName").toString() + ";";
  str += record.value("EndUnitContactName").toString();
  endConnection->setText(str);
  // Set end connection type
  str = record.value("EndConnectionType_Code_FK").toString();
  if(str == "P"){
    endConnection->setConnectionType(mdtClPathGraphicsConnection::Pin);
  }else if(str == "S"){
    endConnection->setConnectionType(mdtClPathGraphicsConnection::Socket);
  }else{
    endConnection->setConnectionType(mdtClPathGraphicsConnection::Terminal);
  }
  // Set link data
  str = record.value("Identification").toString();
  link->setText(str);
  str = record.value("LinkType_Code_FK").toString();
  if(str == "CONNECTION"){
    link->setLinkType(mdtClPathGraphicsLink::Connection);
  }else if(str == "INTERNLINK"){
    link->setLinkType(mdtClPathGraphicsLink::InternalLink);
  }else{
    link->setLinkType(mdtClPathGraphicsLink::CableLink);
  }

  return true;
}
