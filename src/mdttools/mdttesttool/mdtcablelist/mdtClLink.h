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
#ifndef MDT_CL_LINK_H
#define MDT_CL_LINK_H

#include "mdtTtBase.h"
#include "mdtClLinkData.h"
#include "mdtClLinkModificationKeyData.h"
#include "mdtClVehicleTypeLinkKeyData.h"
#include "mdtClArticleLinkData.h"

#include "mdtClUnitConnectionData.h"
#include "mdtClUnitConnectorData.h"

#include "mdtClConnectableCriteria.h" /// \todo remove once ok

#include "mdtSqlTableSelection.h"
#include <QSqlDatabase>
#include <QList>
#include <QVariant>
#include <QString>
#include <QModelIndex>

/*! \brief Helper class for link management
 */
class mdtClLink : public mdtTtBase
{
 public:

  /*! \brief Constructor
   */
  mdtClLink(QObject* parent, QSqlDatabase db);

  /*! \brief Constructor
   */
  mdtClLink(QSqlDatabase db);

  /*! \brief Destructor
   */
  ~mdtClLink();

  /*! \brief Add a link to database
   *
   * \param linkData Link data to add to Link_tbl
   * \param modification Link versionning key data (LinkModification_tbl part)
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool addLink(const mdtClLinkData & linkData, const mdtClLinkModificationKeyData & modification, bool handleTransaction);

  /*! \brief Add a link and assign given vehicle types to it
   *
   * \param linkData Link data to add to Link_tbl
   * \param modification Link versionning key data (LinkModification_tbl part)
   * \param vehicleTypeList List of vehicle types to add in assignations (VehicleType_Link_tbl)
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool addLink(const mdtClLinkData & linkData, const mdtClLinkModificationKeyData & modification, const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction);

  /*! \brief Add a link on base of given article link
   *
   * Will get start and end unit connections that are related to article link's start and end connection
   *  for given unit.
   *  Identification and Resistance are also copied from given article link.
   *  Note that only the Link_tbl part is created here (vehicle types assignations and versionning is not done).
   *
   * \pre articleLinkData must not be null
   * \pre unitId must not be null
   */
  bool addLink(const mdtClArticleLinkData & articleLinkData, const QVariant & unitId);

  /*! \brief Check if a link exists
   */
  bool linkExists(const mdtClLinkPkData & pk, bool & ok);

  /*! \brief Get link data
   *
   * \param pk PK of link for witch to get data
   * \return Data for requested link.
   *          A null data object is returned if requested link does not exist, or a error occured.
   *          Use ok parameter to diffrenciate both cases.
   */
  mdtClLinkData getLinkData(const mdtClLinkPkData & pk, bool & ok);

  /*! \brief Get link data for given article link and star unit and end unit
   */
  mdtClLinkData getLinkData(const mdtClArticleLinkPkData articleLinkPk, const QVariant & startUnitId, const QVariant & endUnitId, bool & ok);

  /*! \brief Update a link, its modification and its vehicle type assignations
   *
   * \param linkPk Link to edit
   * \param linkData New link data
   * \param oldModification Modification to update (LinkModification_tbl part)
   * \param modification New modification (LinkModification_tbl part)
   * \param vehicleTypeList List of vehicle types assignations (VehicleType_Link_tbl)
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   * \pre linkPk must not be null
   * \pre linkData must not be null
   */
  bool updateLink(const mdtClLinkPkData & linkPk, const mdtClLinkData & linkData, 
                  const mdtClLinkModificationKeyData & oldModification, const mdtClLinkModificationKeyData & modification,  
                  const QList<mdtClVehicleTypeStartEndKeyData> & vehicleTypeList, bool handleTransaction);

  /*! \brief Remove a link
   *
   * Will also remove each vehicle types assigned to given link (VehicleType_Link_tbl),
   *  and all modification history (LinkModification_tbl).
   *
   * \param pk PK of link to remove
   * \param handleTransaction Internally, a transaction is (explicitly) open.
   *             By calling this function with a allready open transaction,
   *             set this argument false.
   */
  bool removeLink(const mdtClLinkPkData & pk, bool handleTransaction);

  /*! \brief Remove each unit link that is contained in selection
   *
   * \sa removeLink()
   */
  bool removeLinks(const mdtSqlTableSelection & s);

  /*! \brief Add a link modification (LinkModification_tbl)
   */
  bool addModification(const mdtClLinkModificationKeyData & key);

  /*! \brief Update modification
   */
  bool updateModification(const mdtClLinkModificationKeyData & originalModification, const mdtClLinkModificationKeyData & newModification, bool handleTransaction);

  /*! \brief Remove link modification (LinkModification_tbl)
   */
  bool removeModification(const mdtClLinkModificationKeyData & key);

  /*! \brief Remove all modifications related to given link
   */
  bool removeModifications(const mdtClLinkPkData & linkPk);

  /*! \brief Get a list of connection links between A and B connections, by name
   *
   * Will search connections that have the same name, and that can be connected together, in A and B list.
   *  Currently, connections can be connected if item of A is a pin and item of B is a socket,
   *  or in reverse case.
   *
   * This method is typically used to create connections links between 2 connectors (i.e. connect them).
   *
   * For each link that was found, following fields are also set:
   *  - UnitConnectionStart_Id_FK : ID of item in A
   *  - UnitConnectionEnd_Id_FK : ID of item in B
   *  - LinkType_Code_FK : CONNECTION
   *  - LinkDirection_Code_FK : BID
   *
   * Note: this method is mainly used for unit testing. Consider following helper functions:
   *  - canConnectConnections()
   *  - canConnectConnectors()
   *  - sqlForConnectableUnitConnectorsSelection()
   *  - connectByContactName()
   */
//   QList<mdtClLinkData> getConnectionLinkListByName(const QList<mdtClUnitConnectionData> & A, const QList<mdtClUnitConnectionData> & B, const mdtClConnectableCriteria & criteria);

  /*! \brief Check if unit connection a can be connected with unit connection b
   *
   * \pre A and B muts have connection type set
   */
//   bool canConnectConnections(const mdtClUnitConnectionData & a, const mdtClUnitConnectionData & b, const mdtClConnectableCriteria & criteria);

  /*! \brief Check if unit connection ID A can be connected with unit connection ID B
   */
//   bool canConnectConnections(const mdtClUnitConnectionPkData & pkA, const mdtClUnitConnectionPkData & pkB, const mdtClConnectableCriteria & criteria, bool & ok);
//   bool canConnectConnections(const QVariant & unitConnectionIdA, const QVariant & unitConnectionIdB, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Check if unit connector a can be connected with unit connector b
   */
//   bool canConnectConnectors(const mdtClUnitConnectorData & a, const mdtClUnitConnectorData & b, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Check if unit connector a can be connected with unit connector b
   */
//   bool canConnectConnectors(const mdtClUnitConnectorPkData & pkA, const mdtClUnitConnectorPkData & pkB, const mdtClConnectableCriteria & criteria, bool & ok);
//   bool canConnectConnectors(const QVariant & startUnitConnectorId, const QVariant & endUnitConnectorId, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Get SQL statement for connectable unit connection selection
   *
   * SQL statement will select unit connections that can be connected
   *  to given unitConnectionId.
   *  If given unit ID is not null, only connections related to this unit will be included.
   *
   * \return SQL statement, or empty string on error.
   */
//   QString sqlForConnectableUnitConnectionsSelection(const QVariant & unitConnectionId, const QVariant & unitId, const mdtClConnectableCriteria & criteria);

  /*! \brief Get SQL statement for connectable unit connector selection
   *
   * SQL statement will select unit connectors that can be connected
   *  to given unitConnectorId.
   *  If given unit ID is not null, only connectors related to this unit will be included.
   *
   * \return SQL statement, or empty string on error.
   */
//   QString sqlForConnectableUnitConnectorsSelection(const QVariant & unitConnectorId, const QVariant & unitId, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Get SQL statement for connectable unit connector selection
   *
   * SQL statement will select unit connectors that can be connected
   *  to given unitConnectorId.
   *  If given unit ID list contains IDs, only connectors related to this units will be included.
   *
   * \return SQL statement, or empty string on error.
   */
//   QString sqlForConnectableUnitConnectorsSelection(const QVariant & unitConnectorId, const QList<QVariant> & unitIdList, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Get SQL statement for connectable unit connector's connections selection
   *
   * SQL statement will select unit connection for connectors that can be connected
   *  to given unitConnectorId.
   *  If given unit ID is not null, only connections related to this unit will be included.
   *
   * \return SQL statement, or empty string on error.
   */
  ///QString sqlForConnectableUnitConnectorConnectionsSelection(const QVariant & unitConnectorId, const QVariant & unitId, const mdtClConnectableCriteria & criteria, bool & ok);

  /*! \brief Connect start and end connector by contact name
   *
   * Will create a link of type CONNECTION for each
   *  connection from start connector to a connection of end connector
   *  that can be connected regarding given criteria.
   *
   * For each link that was found, following fields are also set:
   *  - UnitConnectionStart_Id_FK : ID of item in S
   *  - UnitConnectionEnd_Id_FK : ID of item in E
   *  - LinkType_Code_FK : CONNECTION
   *  - LinkDirection_Code_FK : BID
   */
//   bool connectByContactName(const mdtClUnitConnectorData & S, const mdtClUnitConnectorData & E,
//                             const QVariant & startVehicleTypeId, const QVariant & endVehicleTypeId,
//                             const mdtClConnectableCriteria & criteria);

  /*! \brief Connect start and end connector by contact name
   *
   * Will create a link of type CONNECTION for each
   *  connection from start connector to a connection of end connector
   *  that can be connected. See getConnectionLinkListByName() for details.
   */
//   bool connectByContactName(const QVariant & startUnitConnectorId, const QVariant & endUnitConnectorId,
//                             const QVariant & startVehicleTypeId, const QVariant & endVehicleTypeId,
//                             const mdtClConnectableCriteria & criteria);

  /*! \brief Disconnect connector S from E
   *
   * \todo In current version, vehicle type list are not token in acount. This will be the case once removeLink() is fixed.
   */
//   bool disconnectConnectors(const QVariant & startUnitConnectorId, const QVariant & endUnitConnectorId, const QList<QVariant> & startVehicleTypeIdList, const QList<QVariant> & endVehicleTypeIdList);

 private:

  /*! \brief Get a list of connection links between A and B connections, by name
   *
   * Will search connections that have the same name, and that can be connected together, in A and B list
   *  regarding given criteria.
   *
   * This method is typically used to create connections links between 2 connectors (i.e. connect them).
   *
   * If connectionLinkDataList pointer is not null, a list of found links is also created.
   *  In this case, for each link that was found, following fields are also set:
   *  - UnitConnectionStart_Id_FK : ID of item in A
   *  - UnitConnectionEnd_Id_FK : ID of item in B
   *  - LinkType_Code_FK : CONNECTION
   *  - LinkDirection_Code_FK : BID
   */
//   bool checkOrBuildConnectionLinkListByName(const QList<mdtClUnitConnectionData> & A, const QList<mdtClUnitConnectionData> & B, 
//                                             const mdtClConnectableCriteria & criteria, QList<mdtClLinkData> *connectionLinkDataList = 0);

  /*! \brief Fill given record with given data
   */
  void fillRecord(mdtSqlRecord & record, const mdtClLinkData & data);

  /*! \brief Fill given data with given record
   */
  void fillData(mdtClLinkData & data, const QSqlRecord & record);

  Q_DISABLE_COPY(mdtClLink);
};

#endif // MDT_CL_LINK_H
