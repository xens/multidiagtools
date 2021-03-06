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
#ifndef MDT_CL_UNIT_EDITOR_H
#define MDT_CL_UNIT_EDITOR_H

#include <QSqlDatabase>
#include <QObject>
#include <QModelIndex>
#include <QVariant>
#include <QList>
#include <QLocale>
#include "mdtSqlForm.h"
#include "mdtClUnitConnectionData.h"

class QWidget;

/*! \brief Cable list's device editor
 */
class mdtClUnitEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Contruct a cable editor
   */
  mdtClUnitEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Destructor
   */
  ///~mdtClUnitEditor();

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Set working on vehicle type ID list
   */
  ///bool setWorkingOnVehicleTypeIdList(const QList<QVariant> & vtIdList);

 private slots:

  /*! \brief Assign a vehicle to current unit
   */
  void assignVehicle();

  /*! \brief Remove a assigned vehicle to current unit
   */
  void removeVehicleAssignation();

  /*! \brief Set the base article of current unit
   */
  void setBaseArticle();

  /*! \brief Add component
   */
  void addComponent();

  /*! \brief Remove components
   */
  void removeComponents();

  /*! \brief Add a free connector (based on nothing) to unit connector table
   */
  void addConnector();

  /*! \brief Add a connector based on a one from Connector_tbl to unit connector table
   */
  void addConnectorBasedConnector();

  /*! \brief Add a connector based on article connector to unit connector table
   */
  void addArticleConnectorBasedConnector();

  /*! \brief Edit connector name
   */
  void editConnectorName();

  /*! \brief Remove a connector
   */
  void removeConnectors();

  /*! \brief View linked connectors
   */
  void viewLinkedConnectors();

  /*! \brief Add a connection to unit connection table
   */
  void addConnection();

  /*! \brief Add connections based on article connections
   */
  void addArticleConnectionsBasedConnections();

  /*! \brief Edit a connection
   */
  void editConnection();

  /*! \brief Copy function from another connection
   */
  void setFunctionsFromOtherConnection();

  /*! \brief Remove connection from unit connection table
   */
  void removeConnections();

  /*! \brief View linked Connections
   */
  void viewLinkedConnections();

  /*! \brief Add a link
   */
  void addLink();

  /*! \brief Add a link modification
   */
  void addLinkModification();

  /*! \brief Edit selected link
   */
  void editLink();

  /*! \brief Remove  links
   */
  void removeLinks();

  /*! \brief View a path of links
   */
  void viewPath();

  /*! \brief Connect connectors
   */
  void connectConnectors();

 signals:

  /*! \brief Used to tell unit widget that we have edited some data
   */
  void unitEdited();

 private:

  /*! \brief Get current Unit ID
   */
  QVariant currentUnitId();

  /*! \brief Select a vehicle type
   *
   * \return ID of selected vehicle type
   */
  QVariant selectVehicleType(const QString & message, const QString & sql);

  /*! \brief Select a unit
   *
   * \return ID of selected unit
   */
  QVariant selectUnit(const QString & message, const QString & sql);

  /*! \brief Select a unit connector
   *
   * \return ID of selected unit connector
   */
  QVariant selectUnitConnector(const QString & message, const QString & sql);

  /*! \brief Setup Unit table and widget
   */
  bool setupUnitTable();

  /*! \brief Setup unit component table and widget
   */
  bool setupUnitComponentTable();

  /*! \brief Setup unit connector table and widget
   */
  bool setupUnitConnectorTable();

  /*! \brief Setup unit connection table and widget
   */
  bool setupUnitConnectionTable();

  /*! \brief Update unit connection table
   */
  void updateUnitConnectionTable(const QLocale & locale = QLocale());

  /*! \brief Setup unit link table
   */
  bool setupUnitLinkTable();

  /*! \brief Update unit link table
   */
  void updateUnitLinkTable(const QLocale & locale = QLocale());

  /*! \brief Setup VehicleUnit table and widget
   */
  bool setupVehicleTable();

  Q_DISABLE_COPY(mdtClUnitEditor);

  QList<QVariant> pvWorkingOnVehicleTypeIdList;
};

#endif  // #ifndef
