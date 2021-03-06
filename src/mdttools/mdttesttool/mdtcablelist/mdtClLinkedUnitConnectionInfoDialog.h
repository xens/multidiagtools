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
#ifndef MDT_CL_LINKED_UNIT_CONNECTION_INFO_DIALOG_H
#define MDT_CL_LINKED_UNIT_CONNECTION_INFO_DIALOG_H

#include "ui_mdtClLinkedUnitConnectionInfoDialog.h"
#include "mdtError.h"
#include <QDialog>
#include <QSqlDatabase>
#include <QList>
#include <QVariant>
#include <QModelIndex>

class QWidget;
class QSqlQueryModel;
class mdtSortFilterProxyModel;

/*! \brief Display informations about unit connections linked to a given unit connection
 */
class mdtClLinkedUnitConnectionInfoDialog : public QDialog, Ui::mdtClLinkedUnitConnectionInfoDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtClLinkedUnitConnectionInfoDialog(QWidget *parent, QSqlDatabase db);

  /*! \brief Set selection mode flag
   *
   * If selection mode flag is true,
   *  Ok and Cancel buttons are diplayed, and dialog can be accepted.
   *  Else, only close button id displayed.
   */
  void setSelectionModeEnabled(bool enable);

  /*! \brief Set connections
   *
   * \param unitConnectionId ID of unit connection for witch we want to see linked connections
   * \param linkedConnectionIdList List of IDs of linked connections
   */
  void setConnections(const QVariant & unitConnectionId, const QList<QVariant> & linkedConnectionIdList);

  /*! \brief Get function string EN
   */
  QVariant functionStringEN() const;

  /*! \brief Get function string FR
   */
  QVariant functionStringFR() const;

  /*! \brief Get function string DE
   */
  QVariant functionStringDE() const;

  /*! \brief Get function string IT
   */
  QVariant functionStringIT() const;

 private slots:

  /*! \brief Buld function strings and accept dialog (if selection is valid)
   */
  void accept();

 private:

  /*! \brief Update connection info part
   */
  void displayConnection(const QVariant & connectionId);

  /*! \brief Update unit info part
   */
  void displayUnit(const QVariant & unitId);

  /*! \brief Update linked connections part
   */
  void displayLinkedConnections(const QList<QVariant> & connectionIdList);

  /*! \brief Get SQL statement to display linked connections
   */
  QString sqlForLinkedConnections(const QList<QVariant> & connectionIdList) const;

  /*! \brief Display a error
   */
  void displayError(const mdtError & error);

  /*! \brief Build function string EN
   */
  void builFunctionStringEN(const QModelIndex & index);

  /*! \brief Build function string FR
   */
  void builFunctionStringFR(const QModelIndex & index);

  /*! \brief Build function string DE
   */
  void builFunctionStringDE(const QModelIndex & index);

  /*! \brief Build function string IT
   */
  void builFunctionStringIT(const QModelIndex & index);

  Q_DISABLE_COPY(mdtClLinkedUnitConnectionInfoDialog);

  mdtSortFilterProxyModel *pvProxyModel;
  QSqlQueryModel *pvModel;
  QSqlDatabase pvDatabase;
  QString pvFunctionStringEN;
  QString pvFunctionStringFR;
  QString pvFunctionStringDE;
  QString pvFunctionStringIT;
};

#endif // #ifndef MDT_CL_LINKED_UNIT_CONNECTION_INFO_DIALOG_H
