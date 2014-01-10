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
#ifndef MDT_CL_BASE_H
#define MDT_CL_BASE_H

#include "mdtError.h"
#include "mdtSqlRecord.h"
#include <QSqlDatabase>
#include <QString>
#include <QStringList>
#include <QModelIndex>
#include <QVariant>
#include <QList>

/*! \brief Base class for helper class
 *
 * To edit entities (linke Article, Unit, ...) ,
 *  dedicated classes helps for interaction with database .
 *
 * This class is a base that regroups common methods .
 *
 * \todo Should inherit QObject, because:
 *   - Use of tr()
 *   - Use QObject::destroyed() signal (see mdtTtTest for a typical usecase)
 */
class mdtClBase
{
 public:

  /*! \brief Constructor
   */
  mdtClBase(QSqlDatabase db);

  /*! \brief Destructor
   */
  virtual ~mdtClBase();

  /*! \brief Get database instance
   */
  QSqlDatabase database();

  /*! \brief Get last error
   */
  mdtError lastError();

  /*! \brief Add a record to given table
   *
   * Will only get values from fields that have a value.
   *  For this, mdtSqlRecord::hasValue() is used.
   */
  bool addRecord(const mdtSqlRecord & record, const QString & tableName);

  /*! \brief Add a list of records to given table
   *
   * If singleTransaction is true, a transaction will be used for
   *  adding all records. If you need to handle a transaction
   *  outside, set this parameter to false.
   */
  bool addRecordList(const QList<mdtSqlRecord> & recordList, const QString & tableName, bool singleTransaction = false);

  /*! \brief Remove data from given table
   *
   * This method can be used with result of mdtSqlTableWidget::indexListOfSelectedRows()
   *  (the indexes part) .
   *
   * \param tableName Name of table in witch data must be removed .
   * \param fields List of fields. Be shure that this list matches exactly with given indexes .
   * \param indexes In each index, the column will be used to find field name, and data is used for row condition .
   */
  bool removeData(const QString & tableName, const QStringList & fields, const QModelIndexList & indexes);

  /*! \brief Remove data from given table
   *
   * \overload removeData(const QString &, const QStringList &, const QModelIndexList &)
   *
   * This method works for a single field .
   */
  bool removeData(const QString & tableName, const QString & fieldName, const QModelIndexList & indexes);

  /*! \brief Remove data from given table
   *
   * \param tableName Name of table in witch data must be removed .
   * \param fieldName Name of field that contains delete condition .
   * \param matchData Data that match delete condition .
   */
  bool removeData(const QString & tableName, const QString & fieldName, const QVariant & matchData);

 protected:

  /*! \brief Beginn manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool beginTransaction();

  /*! \brief Rollback manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool rollbackTransaction();

  /*! \brief Commit manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool commitTransaction();

  /*! \brief Get SQL data delimiter for given data type
   */
  QString sqlDataDelimiter(QVariant::Type type);

  mdtError pvLastError;

 private:

  Q_DISABLE_COPY(mdtClBase);

  QSqlDatabase pvDatabase;
};

#endif // #ifndef MDTCLBASE_H
