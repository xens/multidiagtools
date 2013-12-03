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
#include "mdtCcTestConnectionCable.h"

mdtCcTestConnectionCable::mdtCcTestConnectionCable(QSqlDatabase db)
 : mdtClBase(db)
{
}

QString mdtCcTestConnectionCable::sqlForStartConnectorSelection(const QVariant & dutUnitId) const
{
  QString sql;

  sql = "SELECT * FROM UnitConnector_tbl ";
  sql += " WHERE Unit_Id_FK = " + dutUnitId.toString();

  return sql;
}

