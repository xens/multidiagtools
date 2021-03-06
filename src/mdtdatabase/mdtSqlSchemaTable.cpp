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
#include "mdtSqlSchemaTable.h"
#include "mdtSqlFieldType.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QSqlField>
#include <QSqlDriver>

//#include <QDebug>

mdtSqlSchemaTable::mdtSqlSchemaTable()
 : pvIsTemporary(false)
{
}

mdtSqlSchemaTable::~mdtSqlSchemaTable()
{
}

void mdtSqlSchemaTable::clear()
{
  pvIsTemporary = false;
  pvDriverType.clear();
  pvDatabaseName.clear();
///  pvTemporaryTableKw.clear();
  pvTableName.clear();
  pvCollation.clear();
  ///pvCharset.clear();
  ///pvCaseSensitivity = Qt::CaseInsensitive;
  pvStorageEngineName.clear();
  pvFields.clear();
  pvPrimaryKey.clear();
  pvIndexes.clear();
 /// pvIndexeAtIsUnique.clear();
  pvForeignKeys.clear();
}

bool mdtSqlSchemaTable::setDriverName(const QString & name) 
{
  if(!pvDriverType.setType(name)){
    pvLastError.setError("Driver name '" + name + "' is not supported.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  return true;
}

void mdtSqlSchemaTable::setDatabaseName(const QString & name) 
{
  pvDatabaseName = name;
}

void mdtSqlSchemaTable::setTableName(const QString & name, const QString &charset, Qt::CaseSensitivity cs) 
{
  pvTableName = name;
  pvCollation.clear();
  pvCollation.setCharset(charset);
  pvCollation.setCaseSensitive(cs == Qt::CaseSensitive);
//   pvCharset = charset.toLower();
//   pvCaseSensitivity = cs;
}

QString mdtSqlSchemaTable::tableName() const
{
  return pvTableName;
}

void mdtSqlSchemaTable::setTemporary(bool temporary)
{
  pvIsTemporary = temporary;
  /**
  if(temporary){
    pvTemporaryTableKw = "TEMPORARY ";
  }else{
    pvTemporaryTableKw.clear();
  }
  */
}

void mdtSqlSchemaTable::setStorageEngineName(const QString & name) 
{
  pvStorageEngineName = name;
}

// void mdtSqlSchemaTable::addField(const mdtSqlField & field, bool isPartOfPrimaryKey)
// {
//   if(isPartOfPrimaryKey){
//     pvPrimaryKey.addField(field);
//   }else{
//     pvFields.append(field);
//   }
// }

void mdtSqlSchemaTable::addField(const mdtSqlField & field, bool isPartOfPrimaryKey)
{
  pvFields.append(field);
  if(isPartOfPrimaryKey){
    pvPrimaryKey.addField(field.name());
  }
}

void mdtSqlSchemaTable::addField(const QSqlField & field, bool isPartOfPrimaryKey) 
{
//   pvFields.append(field);
//   if(isPartOfPrimaryKey){
//     pvPrimaryKey.append(field);
//   }
}

QStringList mdtSqlSchemaTable::getFieldNameList() const
{
  QStringList names;

  for(const auto & field : pvFields){
    names.append(field.name());
  }

  return names;
}

// QSqlField mdtSqlSchemaTable::field(const QString & fieldName) const
// {
//   int i;
// 
//   for(i = 0; i < pvFields.size(); ++i){
//     if(pvFields.at(i).name() == fieldName){
//       return pvFields.at(i);
//     }
//   }
// 
//   return QSqlField();
// }

/*! \todo
 *  see fieldTypeNameMySql() and fieldTypeNameSqlite() and define all properly.
 */
QString mdtSqlSchemaTable::fieldTypeName(int index, mdtSqlDriverType::Type driverType) const
{
  Q_ASSERT(index >= 0);
  Q_ASSERT(index < pvFields.size());

//   if(pvDriverType.type() == mdtSqlDriverType::Unknown){
//     return QString();
//   }
  ///return mdtSqlFieldType::nameFromType(pvFields.at(index).type(), pvDriverType.type());
  
  if(driverType == mdtSqlDriverType::Unknown){
    return QString();
  }
  return mdtSqlFieldType::nameFromType(pvFields.at(index).type(), driverType);
}

void mdtSqlSchemaTable::addIndex(const mdtSqlIndex & index)
{
  pvIndexes.append(index);
}


void mdtSqlSchemaTable::addIndex(const QString & name, bool unique) 
{
//   if(pvIndexes.contains(name)){
//     return;
//   }
//   pvIndexes.insert(name, QSqlIndex("", name));
//   pvIndexeAtIsUnique.insert(name, unique);
}

bool mdtSqlSchemaTable::addFieldToIndex(const QString & indexName, const QString & fieldName) 
{
//   QSqlField field;
//   int i;
// 
//   if(!pvIndexes.contains(indexName)){
//     pvLastError.setError("Table '" + pvTableName + "' : cannot add field '" + fieldName + "' to index '" + indexName\
//                           + "' : index not found.", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return false;
//   }
//   for(i = 0; i < pvFields.size(); ++i){
//     if(pvFields.at(i).name() == fieldName){
//       field = pvFields.at(i);
//       break;
//     }
//   }
//   if(field.name().isEmpty()){
//     pvLastError.setError("Table '" + pvTableName + "' : cannot add field '" + fieldName + "' to index '" + indexName\
//                           + "' : field not found in table.", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return false;
//   }
//   pvIndexes[indexName].append(field);
// 
//   return true;
}

void mdtSqlSchemaTable::addForeignKey(const QString & name, const QString & referingTableName, const foreignKeyAction_t & actionOnDelete, const foreignKeyAction_t & actionOnUpdate) 
{
//   mdtSqlSchemaTableForeignKeyInfo info;
// 
//   if(pvForeignKeys.contains(name)){
//     return;
//   }
//   info.referingTableName = referingTableName;
//   info.actionOnDelete = actionOnDelete;
//   info.actionOnUpdate = actionOnUpdate;
//   pvForeignKeys.insert(name, info);
}

bool mdtSqlSchemaTable::addFieldToForeignKey(const QString & foreignKeyName, const QString & fieldName, const QString & referingFieldName) 
{
//   QSqlField field, referingField;
//   int i;
// 
//   if(!pvForeignKeys.contains(foreignKeyName)){
//     pvLastError.setError("Table '" + pvTableName + "' : cannot add field '" + fieldName + "' to foreign key '" + foreignKeyName\
//                           + "' : foreign key not found.", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return false;
//   }
//   for(i = 0; i < pvFields.size(); ++i){
//     if(pvFields.at(i).name() == fieldName){
//       field = pvFields.at(i);
//     }
//   }
//   if(field.name().isEmpty()){
//     pvLastError.setError("Table '" + pvTableName + "' : cannot add field '" + fieldName + "' to foreign key '" + foreignKeyName\
//                           + "' : field not found in table.", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return false;
//   }
//   referingField.setName(referingFieldName);
//   pvForeignKeys[foreignKeyName].fields.append(field);
//   pvForeignKeys[foreignKeyName].referingFields.append(referingField);
// 
//   return true;
}

bool mdtSqlSchemaTable::updateForeignKeyReferingTable(const QString & name, const QString & referingTableName)
{
//   if(!pvForeignKeys.contains(name)){
//     pvLastError.setError("Table '" + pvTableName + "' : cannot update refering table for foreign key '" + name\
//                           + "' : foreign key not found.", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return false;
//   }
//   pvForeignKeys[name].referingTableName = referingTableName;
// 
//   return true;
}

bool mdtSqlSchemaTable::setupFromTable(const QString & name, QSqlDatabase db)
{
  Q_ASSERT(db.isOpen());

  ///QSqlRecord record;
  ///int i;

  clear();
  // Check that requested table exists
  if(!db.tables(QSql::AllTables).contains(name)){
    pvLastError.setError("Table '" + name + "' not found.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  // Get record that contains field informations
  /**
  record = db.record(name);
  if(record.isEmpty()){
    pvLastError.setError("Table '" + name + "' not found.", mdtError::Error);
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  */
  // Set table and driver names
  pvTableName = name;
  if(!setDriverName(db.driverName())){
    return false;
  }
  // Add fields
  if(!setupFieldsFromDatabase(db)){
    return false;
  }
  /**
  for(i = 0; i < record.count(); ++i){
    pvFields.append(record.field(i));
  }
  */
  // Set primary key
  pvPrimaryKey.setupFromQSqlIndex(db.primaryIndex(name));
  ///pvPrimaryKey = db.primaryIndex(name);
  // Add other indexes
  if(!setupIndexesFromDatabase(db)){
    return false;
  }
  // Add foreign keys
  if(!setupForeignKeysFromDatabase(db)){
    return false;
  }

  return true;
}

QString mdtSqlSchemaTable::getSqlForCreateTable(const QSqlDatabase & db) const
{
  Q_ASSERT(db.isValid());

  QString sql;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);
  bool pkInFieldDefinition;
  int lastFieldIndex = pvFields.size() - 1;

  // Check if we have requiered info
  if(pvTableName.isEmpty()){
    return sql;
  }
  if(pvFields.isEmpty()){
    return sql;
  }
  // Create table beginning part
  if(isTemporary()){
    sql = "CREATE TEMPORARY TABLE ";
  }else{
    sql = "CREATE TABLE ";
  }
  if(!pvDatabaseName.isEmpty()){
    sql += driver->escapeIdentifier(pvDatabaseName, QSqlDriver::TableName) + ".";
  }
  sql += driver->escapeIdentifier(pvTableName, QSqlDriver::TableName) + " (\n";
  // Add fields
  Q_ASSERT(lastFieldIndex >= 0);
  pkInFieldDefinition = false;
  for(int i = 0; i < lastFieldIndex; ++i){
    // If we have exactly 1 field in PK, inclue PK definition in field part
    if( (pvPrimaryKey.fieldCount() == 1) && (pvPrimaryKey.fieldName(0) == pvFields.at(i).name()) ){
      pkInFieldDefinition = true;
    }
    sql += "  " + pvFields.at(i).getSql(db, pkInFieldDefinition) + ",\n";
  }
  sql += "  " + pvFields.at(lastFieldIndex).getSql(db, pkInFieldDefinition) + "\n";
  /// ......
  
  sql += ");\n";
  // Add indexes
  for(auto index : pvIndexes){
    index.setTableName(pvTableName);
    index.generateName();
    sql += index.getSqlForCreate(db, pvDatabaseName);
  }

  return sql;
}

QString mdtSqlSchemaTable::getSqlForDropTable(const QSqlDatabase & db) const
{
  Q_ASSERT(db.isValid());

  QString sql;
  QSqlDriver *driver = db.driver();
  Q_ASSERT(driver != nullptr);

  // Check if we have requiered info
  if(pvTableName.isEmpty()){
    return sql;
  }
  // Build SQL
  sql = "DROP TABLE IF EXISTS ";
  if(!pvDatabaseName.isEmpty()){
    sql += driver->escapeIdentifier(pvDatabaseName, QSqlDriver::TableName) + ".";
  }
  sql += driver->escapeIdentifier(pvTableName, QSqlDriver::TableName) + ";\n";
  // Add indexes
  for(auto index : pvIndexes){
    index.setTableName(pvTableName);
    index.generateName();
    sql += index.getSqlForDrop(db, pvDatabaseName);
  }

  return sql;
}

QString mdtSqlSchemaTable::sqlForCreateTable()
{
  QString sql;

//   // Check if we have requiered info
//   if(pvTableName.trimmed().isEmpty()){
//     return sql;
//   }
//   if(pvFields.isEmpty()){
//     return sql;
//   }
//   // Add the create table statement regarding driver
//   switch(pvDriverType.type()){
//     case mdtSqlDriverType::MariaDB:
//     case mdtSqlDriverType::MySQL:
//       sql += sqlForCreateTableMySql();
//       break;
//     case mdtSqlDriverType::SQLite:
//       sql += sqlForCreateTableSqlite();
//       break;
//     case mdtSqlDriverType::Unknown:
//       pvLastError.setError("Unknown driver name '" + pvDriverType.name() + "'", mdtError::Error);
//       MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//       pvLastError.commit();
//       break;
//   }

  return sql;
}

QString mdtSqlSchemaTable::sqlForDropTable() const
{
  QString sql;
//   QString delimiter;

//   if(pvTableName.isEmpty()){
//     return "";
//   }
//   switch(pvDriverType.type()){
//     case mdtSqlDriverType::MariaDB:
//     case mdtSqlDriverType::MySQL:
//       delimiter = "`";
//       break;
//     case mdtSqlDriverType::SQLite:
//       delimiter = "'";
//       break;
//     case mdtSqlDriverType::Unknown:
//       break;
//   }
//   if(pvDatabaseName.trimmed().isEmpty()){
//     sql = "DROP TABLE IF EXISTS " + delimiter + pvTableName + delimiter + ";\n";
//   }else{
//     sql = "DROP TABLE IF EXISTS " + delimiter + pvDatabaseName + delimiter + "." + delimiter + pvTableName + delimiter + ";\n";
//   }

  return sql;
}

mdtError mdtSqlSchemaTable::lastError() const
{
  return pvLastError;
}

QString mdtSqlSchemaTable::sqlForCreateTableMySql() const
{
  QString sql;

//   if(pvDatabaseName.trimmed().isEmpty()){
//     ///sql = "CREATE " + pvTemporaryTableKw + "TABLE `" + pvTableName + "` (\n";
//   }else{
//     ///sql = "CREATE " + pvTemporaryTableKw + "TABLE `" + pvDatabaseName + "`.`" + pvTableName + "` (\n";
//   }
//   // Add fields
//   sql += sqlForFieldsMySql();
//   if((pvPrimaryKey.isEmpty())&&(pvIndexes.isEmpty())&&(pvForeignKeys.isEmpty())){
//     sql += "\n";
//   }
//   // Add primary key
//   sql += sqlForPrimaryKey("`");
//   // Add indexes
//   sql += sqlForIndexesMySql();
//   // Add foreign keys
//   sql += sqlForForeignKeys("`");
//   // Add end + table attributes
//   sql += "\n)";
//   if(!pvStorageEngineName.trimmed().isEmpty()){
//     sql += " ENGINE=" + pvStorageEngineName;
//   }
//   /**
//   if(!pvCharset.trimmed().isEmpty()){
//     sql += " DEFAULT CHARSET=" + pvCharset;
//   }
//   */
//   if(pvDriverType.type() == mdtSqlDriverType::MySQL){
//     sql += sqlForCollateMySql();
//   }
//   sql += ";\n";

  return sql;
}

QString mdtSqlSchemaTable::sqlForCreateTableSqlite() const
{
  QString sql;
//   QSqlField field;
// 
//   if(pvDatabaseName.trimmed().isEmpty()){
//     ///sql = "CREATE " + pvTemporaryTableKw + "TABLE '" + pvTableName + "' (\n";
//   }else{
//     ///sql = "CREATE " + pvTemporaryTableKw + "TABLE '" + pvDatabaseName + "'.'" + pvTableName + "' (\n";
//   }
//   // Add fields
//   sql += sqlForFieldsSqlite();
//   if((pvPrimaryKey.isEmpty())&&(pvIndexes.isEmpty())&&(pvForeignKeys.isEmpty())){
//     sql += "\n";
//   }
//   // Add primary key
//   if(!((pvPrimaryKey.count() == 1)&&(pvPrimaryKey.field(0).type() == QVariant::Int)&&(pvPrimaryKey.field(0).isAutoValue()))){
//     sql += sqlForPrimaryKey("'");
//   }
//   // Add indexes
//   sql += sqlForIndexesSqlite();
//   // Add foreign keys
//   sql += sqlForForeignKeys("'");
//   // Add end + table attributes
//   sql += "\n);\n";

  return sql;
}

QString mdtSqlSchemaTable::sqlForFieldsMySql() const
{
  QString sql;
//   QSqlField field;
//   int i;
// 
//   for(i = 0; i < pvFields.size(); ++i){
//     field = pvFields.at(i);
//     sql += "  `" + field.name() + "` ";
//     sql += fieldTypeNameMySql(field.type(), field.length());
//     if((field.requiredStatus() == QSqlField::Required)||(pvPrimaryKey.contains(field.name()))){
//       sql += " NOT NULL";
//     }
//     if((pvPrimaryKey.contains(field.name()))&&(field.isAutoValue())){
//       sql += " AUTO_INCREMENT";
//     }else{
//       if(field.defaultValue().isNull()){
//         sql += " DEFAULT NULL";
//       }else{
//         sql += " DEFAULT " + field.defaultValue().toString();
//       }
//     }
//     if(i < (pvFields.size() - 1)){
//       sql += ",\n";
//     }
//   }

  return sql;
}

QString mdtSqlSchemaTable::sqlForFieldsSqlite() const
{
  QString sql;
//   QSqlField field;
//   int i;
// 
//   for(i = 0; i < pvFields.size(); ++i){
//     field = pvFields.at(i);
//     sql += "  '" + field.name() + "' ";
//     // Auto increment is only accepted for one case and needs a special syntax in Sqlite
//     if((pvPrimaryKey.count() == 1)&&(field.type() == QVariant::Int)&&(field.isAutoValue())&&(pvPrimaryKey.contains(field.name()))){
//       sql += "INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT";
//     }else{
//       sql += fieldTypeNameSqlite(field.type(), field.length());
//       if((field.requiredStatus() == QSqlField::Required)||(pvPrimaryKey.contains(field.name()))){
//         sql += " NOT NULL";
//       }
//       if(field.defaultValue().isNull()){
//         sql += " DEFAULT NULL";
//       }else{
//         sql += " DEFAULT " + field.defaultValue().toString();
//       }
//     }
//     if(field.type() == QVariant::String){
//       sql += sqlForCollateSqlite();
//     }
//     if(i < (pvFields.size() - 1)){
//       sql += ",\n";
//     }
//   }

  return sql;
}

QString mdtSqlSchemaTable::sqlForPrimaryKey(const QString &delimiter) const
{
  QString sql;
  int i;

//   if(pvPrimaryKey.count() > 0){
//     sql += ",\n";
//     sql += "  PRIMARY KEY (";
//     for(i = 0; i < pvPrimaryKey.count(); ++i){
//       sql += delimiter + pvPrimaryKey.field(i).name() + delimiter;
//       if(i < (pvPrimaryKey.count() - 1)){
//         sql += ",";
//       }
//     }
//     sql += ")";
//   }

  return sql;
}

QString mdtSqlSchemaTable::sqlForIndexesMySql() const
{
  QString sql;
//   QSqlIndex index;
//   int i;
// 
//   if(pvIndexes.isEmpty()){
//     return "";
//   }
//   sql = ",\n";
//   QHashIterator<QString, QSqlIndex> it(pvIndexes);
//   while(it.hasNext()){
//     it.next();
//     index = it.value();
//     if(pvIndexeAtIsUnique.value(index.name())){
//       sql += "  UNIQUE INDEX `" + index.name() + "` (";
//     }else{
//       sql += "  INDEX `" + index.name() + "` (";
//     }
//     for(i = 0; i < index.count(); ++i){
//       sql += "`" + index.field(i).name() + "`";
//       if(i < (index.count() - 1)){
//         sql += ",";
//       }
//     }
//     if(it.hasNext()){
//       sql += "),\n";
//     }else{
//       sql += ")";
//     }
//   }

  return sql;
}

QString mdtSqlSchemaTable::sqlForIndexesSqlite() const
{
  QString sql, indexSql;
//   QSqlIndex index;
//   QHash<QString, QSqlIndex> uniqueIndexes;
//   int i;
// 
//   // With Sqlite, only unique indexes must be taken into account
//   QHashIterator<QString, QSqlIndex> it(pvIndexes);
//   while(it.hasNext()){
//     it.next();
//     if(pvIndexeAtIsUnique.value(it.value().name())){
//       uniqueIndexes.insert(it.key(), it.value());
//     }
//   }
//   if(uniqueIndexes.isEmpty()){
//     return "";
//   }
//   QHashIterator<QString, QSqlIndex> it2(uniqueIndexes);
//   while(it2.hasNext()){
//     it2.next();
//     index = it2.value();
//     ///if(pvIndexeAtIsUnique.value(index.name())){
//     indexSql += "  UNIQUE (";
//     for(i = 0; i < index.count(); ++i){
//       indexSql += "'" + index.field(i).name() + "'";
//       if(i < (index.count() - 1)){
//         indexSql += ",";
//       }
//     }
//     if(it2.hasNext()){
//       indexSql += "),\n";
//     }else{
//       indexSql += ")";
//     }
//     ///}
//   }
//   if(!indexSql.trimmed().isEmpty()){
//     sql = ",\n" + indexSql;
//   }

  return sql;
}

QString mdtSqlSchemaTable::sqlForForeignKeys(const QString &delimiter) const
{
//   Q_ASSERT(!pvDriverType.isNull());
// 
//   QString sql;
//   QSqlField field;
//   QSqlRecord fields;
//   mdtSqlSchemaTableForeignKeyInfo fkInfo;
//   int i;
// 
//   if(pvForeignKeys.isEmpty()){
//     return "";
//   }
//   sql = ",\n";
//   QHashIterator<QString, mdtSqlSchemaTableForeignKeyInfo> itf(pvForeignKeys);
//   while(itf.hasNext()){
//     itf.next();
//     fkInfo = itf.value();
//     switch(pvDriverType.type()){
//       case mdtSqlDriverType::MariaDB:
//       case mdtSqlDriverType::MySQL:
//         sql += "  FOREIGN KEY " + delimiter + itf.key() + delimiter + " (";
//         break;
//       case mdtSqlDriverType::SQLite:
//         sql += "  FOREIGN KEY (";
//         break;
//       case mdtSqlDriverType::Unknown:
//         // Should never happen (we put it just to avoid compiler warnings, and we don't want a default in switch)
//         break;
//     }
//     fields = fkInfo.fields;
//     for(i = 0; i < fields.count(); ++i){
//       sql += delimiter + fields.field(i).name() + delimiter;
//       if(i < (fields.count() - 1)){
//         sql += ",";
//       }
//     }
//     sql += ")\n";
//     sql += "   REFERENCES " + delimiter + fkInfo.referingTableName + delimiter + " (";
//     fields = fkInfo.referingFields;
//     for(i = 0; i < fields.count(); ++i){
//       sql += delimiter + fields.field(i).name() + delimiter;
//       if(i < (fields.count() - 1)){
//         sql += ",";
//       }
//     }
//     sql += ")\n";
//     sql += "   ON DELETE " + foreignKeyActionName((foreignKeyAction_t)fkInfo.actionOnDelete) + "\n";
//     sql += "   ON UPDATE " + foreignKeyActionName((foreignKeyAction_t)fkInfo.actionOnUpdate);
//     if(itf.hasNext()){
//       sql += ",\n";
//     }
//   }
// 
//   return sql;
}

QString mdtSqlSchemaTable::fieldTypeNameMySql(QVariant::Type type, int length) const
{
  QString strType;

//   switch(type){
//     case QVariant::Int:
//       strType = "INT";
//       break;
//     case QVariant::UInt:
//       strType = "INT";
//       break;
//     case QVariant::LongLong:
//       strType = "BIGINT";
//       break;
//     case QVariant::ULongLong:
//       strType = "BIGINT";
//       break;
//     case QVariant::Double:
//       strType = "DOUBLE PRECISION";
//       break;
//     case QVariant::Bool:
//       strType = "BOOL";
//       break;
//     case QVariant::Date:
//       strType = "DATE";
//       break;
//     case QVariant::Time:
//       strType = "TIME";
//       break;
//     case QVariant::DateTime:
//       strType = "DATETIME";
//       break;
//     case QVariant::String:
//       strType = "VARCHAR";
//       break;
//     case QVariant::Bitmap:
//       strType = "BLOB";
//       break;
//     case QVariant::Image:
//       strType = "BLOB";
//       break;
//     default:
//       mdtError e(MDT_DATABASE_ERROR, "Unknown mapping for type " + QString::number(type) + " (QVariant::Type)", mdtError::Error);
//       MDT_ERROR_SET_SRC(e, "mdtSqlSchemaTable");
//       e.commit();
//   }
//   // Add length if defined (We not check if length is supported for given type)
//   if(length > 0){
//     strType += "(" + QString::number(length) + ")";
//   }
//   // Add the UNSIGNED suffix if type is unsigned
//   if((type == QVariant::UInt)||(type == QVariant::ULongLong)){
//     strType += " UNSIGNED";
//   }

  return strType;
}

QString mdtSqlSchemaTable::fieldTypeNameSqlite(QVariant::Type type, int length) const
{
  QString strType;

//   switch(type){
//     case QVariant::Int:
//       strType = "INT";
//       break;
//     case QVariant::UInt:
//       strType = "INT";
//       break;
//     case QVariant::LongLong:
//       strType = "BIGINT";
//       break;
//     case QVariant::ULongLong:
//       strType = "BIGINT";
//       break;
//     case QVariant::Double:
//       strType = "DOUBLE";
//       break;
//     case QVariant::Bool:
//       strType = "BOOLEAN";
//       break;
//     case QVariant::Date:
//       strType = "DATE";
//       break;
//     case QVariant::Time:
//       strType = "DATETIME";
//       break;
//     case QVariant::DateTime:
//       strType = "DATETIME";
//       break;
//     case QVariant::String:
//       strType = "VARCHAR";
//       break;
//     case QVariant::Bitmap:
//       strType = "BLOB";
//       break;
//     case QVariant::Image:
//       strType = "BLOB";
//       break;
//     default:
//       mdtError e(MDT_DATABASE_ERROR, "Unknown mapping for type " + QString::number(type) + " (QVariant::Type)", mdtError::Error);
//       MDT_ERROR_SET_SRC(e, "mdtSqlSchemaTable");
//       e.commit();
//   }
  ///strType = mdtSqlFieldType::nameFromType(type, mdtSqlDriverType::SQLite);
//   if(strType.isEmpty()){
//     pvLastError.setError("Unknown mapping for type " + QString::number(type) + " (QVariant::Type)", mdtError::Error);
//     MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
//     pvLastError.commit();
//     return strType;
//   }
//   // Add length if defined (We not check if length is supported for given type)
//   if(length > 0){
//     strType += "(" + QString::number(length) + ")";
//   }
//   // Add the UNSIGNED suffix if type is unsigned
//   if((type == QVariant::UInt)||(type == QVariant::ULongLong)){
//     strType += " UNSIGNED";
//   }

  return strType;
}

QString mdtSqlSchemaTable::sqlForCollateMySql() const
{
  QString sql;

  /**
  if(pvCharset.trimmed().isEmpty()){
    return sql;
  }
  sql = " COLLATE=";
  // Set first part regarding charset
  if(pvCharset.toLower() == "utf8"){
    sql += "utf8_unicode";
  }
  // Set case sensitivity
  switch(pvCaseSensitivity){
    case Qt::CaseInsensitive:
      sql += "_ci";
      break;
    case Qt::CaseSensitive:
      sql += "_cs";
      break;
  }
  */

  return sql;
}

QString mdtSqlSchemaTable::sqlForCollateSqlite() const
{
  /**
  switch(pvCaseSensitivity){
    case Qt::CaseInsensitive:
      return " COLLATE NOCASE";
    case Qt::CaseSensitive:
      return " COLLATE BINARY";
      break;
  }
  */

  return "";
}

QString mdtSqlSchemaTable::foreignKeyActionName(foreignKeyAction_t action) const
{
//   switch(action){
//     case Cascade:
//       return "CASCADE";
//     case SetNull:
//       return "SET NULL";
//     case NoAction:
//       return "NO ACTION";
//     case Restrict:
//       return "RESTRICT";
//   }
  return "";
}

mdtSqlSchemaTable::foreignKeyAction_t mdtSqlSchemaTable::foreignKeyActionFromName(const QString & name) const
{
//   if(name.toUpper() == "CASCADE"){
//     return Cascade;
//   }
//   if(name.toUpper() == "SET NULL"){
//     return SetNull;
//   }
//   if(name.toUpper() == "RESTRICT"){
//     return Restrict;
//   }
//   return NoAction;
}

bool mdtSqlSchemaTable::setupFieldsFromDatabase(const QSqlDatabase & db)
{
  switch(pvDriverType.type()){
    case mdtSqlDriverType::Unknown:
      return false;
    case mdtSqlDriverType::SQLite:
      return setupFieldsFromDatabaseSqlite(db);
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
      break;
  }
  QSqlRecord record = db.record(pvTableName);
  int i;
  Q_ASSERT(!record.isEmpty());
  for(i = 0; i < record.count(); ++i){
    ///pvFields.append(record.field(i));
  }
  return true;
}

bool mdtSqlSchemaTable::setupFieldsFromDatabaseSqlite(const QSqlDatabase & db)
{
  QSqlRecord dbRecord = db.record(pvTableName);
  QSqlRecord record;
  QSqlQuery query(db);
  QString sql;
  QSqlField field;
  QString fieldTypeName;

  Q_ASSERT(!dbRecord.isEmpty());

  // Get infos from database
  sql = "PRAGMA table_info(" + pvTableName + ")";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot get informations for table '" + pvTableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  // Add fields
  while(query.next()){
    record = query.record();
    // Most of setup can be done via Qt's driver
    field = dbRecord.field(record.value("name").toString());
    // If field type is one that we want to handle, set it
    fieldTypeName = record.value("type").toString().toUpper();
    if(fieldTypeName == "DATE"){
      field.setType(QVariant::Date);
    }else if(fieldTypeName == "TIME"){
      field.setType(QVariant::Time);
    }else if(fieldTypeName == "DATETIME"){
      field.setType(QVariant::DateTime);
    }
    // Add field
    ///pvFields.append(field);
  }

  return true;
}

bool mdtSqlSchemaTable::setupIndexesFromDatabase(const QSqlDatabase & db)
{
  switch(pvDriverType.type()){
    case mdtSqlDriverType::SQLite:
      return setupIndexesFromDatabaseSqlite(db);
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
    case mdtSqlDriverType::Unknown:
      break;
  }
  pvLastError.setError("Unknown driver name '" + pvDriverType.name() + "'", mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
  pvLastError.commit();
  return false;
}

bool mdtSqlSchemaTable::setupIndexesFromDatabaseSqlite(const QSqlDatabase & db)
{
  QSqlQuery query(db);
  QString sql;
  QSqlRecord record;
  QString indexName;
  QStringList indexList;
  int i;

  // Get index list
  sql = "PRAGMA index_list(" + pvTableName + ")";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot get list of indexes for table '" + pvTableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  // Add indexes
  while(query.next()){
    record = query.record();
    indexName = record.value("name").toString();
    addIndex(indexName, record.value("unique").toBool());
    indexList.append(indexName);
  }
  // Add fields for each indexes
  for(i = 0; i < indexList.size(); ++i){
    indexName = indexList.at(i);
    sql = "PRAGMA index_info(" + indexName + ")";
    if(!query.exec(sql)){
      QSqlError sqlError = query.lastError();
      pvLastError.setError("Cannot get index info for index '" + indexName + "' in table '" + pvTableName + "'", mdtError::Error);
      pvLastError.setSystemError(sqlError.number(), sqlError.text());
      MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
      pvLastError.commit();
      return false;
    }
    while(query.next()){
      record = query.record();
      if(!addFieldToIndex(indexName, record.value("name").toString())){
        return false;
      }
    }
  }

  return true;
}

bool mdtSqlSchemaTable::setupForeignKeysFromDatabase(const QSqlDatabase & db)
{
  switch(pvDriverType.type()){
    case mdtSqlDriverType::SQLite:
      return setupForeignKeysFromDatabaseSqlite(db);
    case mdtSqlDriverType::MariaDB:
    case mdtSqlDriverType::MySQL:
    case mdtSqlDriverType::Unknown:
      break;
  }
  pvLastError.setError("Unknown driver name '" + pvDriverType.name() + "'", mdtError::Error);
  MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
  pvLastError.commit();
  return false;
}

bool mdtSqlSchemaTable::setupForeignKeysFromDatabaseSqlite(const QSqlDatabase & db)
{
  QSqlQuery query(db);
  QString sql;
  QSqlRecord record;
  QString fkName;

  // Get foreign key list
  sql = "PRAGMA foreign_key_list(" + pvTableName + ")";
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot get list of foreign keys for table '" + pvTableName + "'", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtSqlSchemaTable");
    pvLastError.commit();
    return false;
  }
  // Add foreign keys
  while(query.next()){
    record = query.record();
    fkName = record.value("from").toString() + "_fk";
    addForeignKey(fkName, record.value("table").toString(), foreignKeyActionFromName(record.value("on_delete").toString()), foreignKeyActionFromName(record.value("on_update").toString()) );
    if(!addFieldToForeignKey(fkName, record.value("from").toString(), record.value("to").toString())){
      return false;
    }
  }

  return true;
}
