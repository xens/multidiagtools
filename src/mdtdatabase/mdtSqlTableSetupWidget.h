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
#ifndef MDT_SQL_TABLE_SETUP_WIDGET_H
#define MDT_SQL_TABLE_SETUP_WIDGET_H

#include "ui_mdtSqlTableSetupWidget.h"
#include <QWidget>

class mdtSqlTableSchemaModel;

/*! \brief SQL table schema edition
 *
 * Display SQL table schema in a table view and let user add, remove and edit fields
 */
class mdtSqlTableSetupWidget : public QWidget, Ui::mdtSqlTableSetupWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtSqlTableSetupWidget(QWidget *parent = nullptr);

 private:

  Q_DISABLE_COPY(mdtSqlTableSetupWidget)

  mdtSqlTableSchemaModel *pvSchemaTabledModel;
};

#endif // #ifndef MDT_SQL_TABLE_SETUP_WIDGET_H
