/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#ifndef MDT_CL_TERMINAL_BLOCK_EDITOR_H
#define MDT_CL_TERMINAL_BLOCK_EDITOR_H

#include <QSqlDatabase>
#include <QObject>
#include <QSqlError>

class mdtSqlWindowOld;
///class mdtSqlFormWidget;
class mdtSqlTableWidget;
class mdtSqlRelation;
class QSqlTableModel;
class QPushButton;

/*! \brief Cable list's device editor
 */
class mdtClTerminalBlockEditor : public QObject
{
 Q_OBJECT

 public:

  /*! \brief Contruct a cable editor
   */
  mdtClTerminalBlockEditor(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

  /*! \brief Destructor
   */
  ~mdtClTerminalBlockEditor();

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Setup GUI
   *
   * Will dispose internal widgets into window
   *
   * \pre window must be a valid pointer
   */
  void setupUi(mdtSqlWindowOld *window);

 private slots:

  /*! \brief Assign a vehicle to current unit
   */
  void assignVehicle();

  /*! \brief Remove a assigned vehicle to current unit
   */
  void removeVehicleAssignation();

  /*! \brief Add a terminal to unit connection table
   */
  void addTerminal();

  /*! \brief Remove terminal from unit connection table
   */
  void removeTerminal();

 signals:

  /*! \brief Used to tell unit widget that we have edited some data
   */
  void unitEdited();

 private:

  /*! \brief Get current Unit ID
   *
   * Will return current ID from Unit table.
   *  Returns a value < 0 on error (no row, ...)
   */
  int currentUnitId();

  /*! \brief Setup Unit table and widget
   */
  bool setupUnitTable();

  /*! \brief Setup terminal ( = unit connection) edit table and widget
   */
  bool setupTerminalEditTable();

  /*! \brief Setup terminal link edit table and widget
   */
  bool setupTerminalLinkEditTable();

  /*! \brief Setup VehicleUnit table and widget
   */
  bool setupVehicleTable();

  /*! \brief Display error to the user
   */
  void displayError(const QString &title, const QString &text, const QSqlError &error, bool log);

  Q_DISABLE_COPY(mdtClTerminalBlockEditor);

  QSqlDatabase pvDatabase;
  // Unit objects
  ///mdtSqlFormWidget *pvUnitWidget;
  QSqlTableModel *pvUnitModel;
  // Terminal ( = Unit connection) view objects
  mdtSqlTableWidget *pvTerminalEditWidget;
  QSqlTableModel *pvTerminalEditModel;
  mdtSqlRelation *pvTerminalEditRelation;
  // Terminal links
  mdtSqlTableWidget *pvTerminalLinkEditWidget;
  QSqlTableModel *pvTerminalLinkEditModel;
  mdtSqlRelation *pvTerminalLinkEditRelation;
  // Vehicle objects
  mdtSqlTableWidget *pvVehicleTypeWidget;
  QSqlTableModel *pvVehicleTypeModel;
  mdtSqlRelation *pvVehicleTypeRelation;
};

#endif  // #ifndef MDT_CL_TERMINAL_BLOCK_EDITOR_H
