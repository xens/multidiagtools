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
#include "mdtSqlField.h"
#include <QSqlDriver>
#include <QSqlDatabase>

void mdtSqlField::clear()
{
  pvType = mdtSqlFieldType::UnknownType;
  pvName.clear();
  pvIsAutoValue = false;
  pvIsRequired = false;
  pvDefaultValue.clear();
  pvLength = -1;
  pvCollation.clear();
}

void mdtSqlField::setupFromQSqlField(const QSqlField & qtField, mdtSqlDriverType::Type driverType)
{
  pvType = mdtSqlFieldType::fromQVariantType(qtField.type(), driverType);
  pvName = qtField.name();
  pvIsAutoValue = qtField.isAutoValue();
  pvIsRequired = (qtField.requiredStatus() == QSqlField::Required);
  pvDefaultValue = qtField.defaultValue();
  pvLength = qtField.length();
}

QString mdtSqlField::getSql(const QSqlDatabase & db, bool pk) const
{
  Q_ASSERT(db.driver() != nullptr);

  QString sql;
  mdtSqlDriverType::Type driverType = mdtSqlDriverType::typeFromName(db.driverName());
  QSqlDriver *driver = db.driver();

  // Field name
  sql = driver->escapeIdentifier(pvName, QSqlDriver::FieldName);
  // Field type
  sql += " " + mdtSqlFieldType::nameFromType(pvType, driverType);
  if(pvLength > 0){
    sql += "(" + QString::number(pvLength) + ")";
  }
  // Other definitions witch are databse specific
  switch(driverType){
    case mdtSqlDriverType::SQLite:
      sql += getSqliteDefinition(pk);
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
    case mdtSqlDriverType::Unknown:
      break;
  }

  return sql;
}

QString mdtSqlField::getSqliteDefinition(bool pk) const
{
  QString sql;

  /*
   * Is SQLite, auto increment PK is a special case
   */
  if( (pk) && (pvIsAutoValue) && (pvType == mdtSqlFieldType::Integer) ){
    sql += " NOT NULL PRIMARY KEY AUTOINCREMENT";
    return sql;
  }
  // Null constraint
  if(pvIsRequired){
    sql += " NOT NULL";
  }
  // Default value
  if(pvDefaultValue.isNull()){
    sql += " DEFAULT NULL";
  }else{
    sql += " DEFAULT " + pvDefaultValue.toString();
  }
  // Collation
  if(!pvCollation.isNull()){
    sql += " COLLATE " + pvCollation.getSql(mdtSqlDriverType::SQLite);
  }

  return sql;
}
