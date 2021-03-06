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
#include "mdtClArticle.h"
#include "mdtError.h"
#include "mdtClConnectorData.h"
#include <QString>
#include <QStringList>
#include <QSqlQuery>

//#include <QDebug>

mdtClArticle::mdtClArticle(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClArticle::~mdtClArticle()
{
}

QString mdtClArticle::sqlForArticleComponentSelection(const QVariant &articleId) const
{
  QString sql;

  sql =  "SELECT Id_PK, ArticleCode, Unit, DesignationEN, DesignationFR, DesignationDE, DesignationIT "\
         "FROM Article_tbl "\
         "WHERE ( Id_PK <> " + articleId.toString() + " ) "\
         "AND ( Id_PK NOT IN ( "\
         " SELECT Component_Id_FK "\
         " FROM ArticleComponent_tbl "\
         " WHERE Composite_Id_FK = " + articleId.toString() + " ) "\
         " ) ";

  return sql;
}

bool mdtClArticle::addComponent(const QVariant &articleId, const QVariant &componentId, const QVariant &qty, const QVariant &qtyUnit)
{
  QString sql;
  QSqlQuery query(database());

  // Prepare query for insertion
  sql = "INSERT INTO ArticleComponent_tbl (Composite_Id_FK, Component_Id_FK, ComponentQty, ComponentQtyUnit) "\
        "VALUES (:Composite_Id_FK, :Component_Id_FK, :ComponentQty, :ComponentQtyUnit)";
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for component inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Composite_Id_FK", articleId);
  query.bindValue(":Component_Id_FK", componentId);
  query.bindValue(":ComponentQty", qty);
  query.bindValue(":ComponentQtyUnit", qtyUnit);
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for component inertion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClArticle::editComponent(const QVariant &articleId, const QVariant &currentComponentId, const QVariant &newComponentId, const QVariant &qty, const QVariant &qtyUnit)
{
  QString sql;
  QSqlQuery query(database());

  // Prepare query for edition
  sql = "UPDATE ArticleComponent_tbl "\
        "SET Component_Id_FK = :Component_Id_FK , "\
            "ComponentQty = :ComponentQty , "\
            "ComponentQtyUnit = :ComponentQtyUnit "\
        "WHERE ( Composite_Id_FK = " + articleId.toString();
  sql += " AND   Component_Id_FK = " + currentComponentId.toString() + ")";
  if(!query.prepare(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot prepare query for component edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }
  // Add values and execute query
  query.bindValue(":Component_Id_FK", newComponentId);
  query.bindValue(":ComponentQty", qty);
  query.bindValue(":ComponentQtyUnit", qtyUnit);
  if(!query.exec()){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for component edition", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}


bool mdtClArticle::removeComponent(const QVariant &articleId, const QVariant &componentId)
{
  return removeData("ArticleComponent_tbl", "Composite_Id_FK", articleId, "Component_Id_FK", componentId);
}

bool mdtClArticle::removeComponents(const QVariant &articleId, const QList<QVariant> &componentIdList)
{
  int i;
  QString sql;

  if(componentIdList.size() < 1){
    return true;
  }
  // Generate SQL
  sql = "DELETE FROM ArticleComponent_tbl ";
  for(i = 0; i < componentIdList.size(); ++i){
    if(i == 0){
      sql += " WHERE ( ";
    }else{
      sql += " OR ";
    }
    sql += " ( Component_Id_FK = " + componentIdList.at(i).toString();
    sql += " AND Composite_Id_FK = " + articleId.toString() + " ) ";
  }
  sql += " ) ";
  // Submit query
  QSqlQuery query(database());
  if(!query.exec(sql)){
    QSqlError sqlError = query.lastError();
    pvLastError.setError("Cannot execute query for component deletion", mdtError::Error);
    pvLastError.setSystemError(sqlError.number(), sqlError.text());
    MDT_ERROR_SET_SRC(pvLastError, "mdtClArticle");
    pvLastError.commit();
    return false;
  }

  return true;
}

bool mdtClArticle::removeComponents(const QVariant &articleId, const mdtSqlTableSelection & s)
{
  QList<QVariant> idList;

  idList = s.dataList("Component_Id_PK");

  return removeComponents(articleId, idList);
}
