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
#ifndef MDT_TT_TEST_SYSTEM_COMPONENT_EDITOR_H
#define MDT_TT_TEST_SYSTEM_COMPONENT_EDITOR_H

#include "mdtSqlForm.h"
#include <QSqlDatabase>
#include <QList>
#include <QLocale>

class QWidget;

/*! \brief Test system component editor
 */
class mdtTtTestSystemComponentEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestSystemComponentEditor(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup tables
   */
  bool setupTables();

 private slots:

  /*! \brief Add test system assignation
   */
  void addTestSystemAssignation();

  /*! \brief Remove test system assignation
   */
  void removeTestSystemAssignation();

 private:

  /*! \brief Setup TestSystemComponent_tbl part
   */
  bool setupTestSystemComponentTable();

  /*! \brief Setup TestSystemOfComponent_view
   */
  bool setupTestSystemAssignationTable();

  Q_DISABLE_COPY(mdtTtTestSystemComponentEditor);
};

#endif // #ifndef MDT_TT_TEST_SYSTEM_COMPONENT_EDITOR_H
