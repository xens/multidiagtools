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
#ifndef MDT_SQL_FOREIGN_KEY_H
#define MDT_SQL_FOREIGN_KEY_H

#include <QString>
#include <QStringList>

class QSqlDatabase;

/*! \brief Foreign key information for SQL database schema creation
 */
class mdtSqlForeignKey
{
 public:

  /*! \brief Foreign key action
   */
  enum Action : uint8_t
  {
    NoAction,   /*!< No action */
    Restrict,   /*!< Restrict */
    SetNull,    /*!< Set NULL */
    SetDefault, /*!< Set default */
    Cascade     /*!< Cascade */
  };

  /*! \brief Default constructor
   */
  mdtSqlForeignKey()
   : pvCreateParentIndex(false),
     pvCreateChildIndex(false),
     pvOnDeleteAction(NoAction),
     pvOnUpdateAction(NoAction)
   {
   }

  /*! \brief Set parent table name
   */
  void setParentTableName(const QString & name)
  {
    pvParentTableName = name;
  }

  /*! \brief Get parent table name
   */
  QString parentTableName() const
  {
    return pvParentTableName;
  }

  /*! \brief Set child table name
   */
  void setChildTableName(const QString & name)
  {
    pvChildTableName = name;
  }

  /*! \brief Get child table name
   */
  QString childTableName() const
  {
    return pvChildTableName;
  }

  /*! \brief Add a couple of fields
   */
  void addKeyFields(const QString & parentTableFieldName, const QString & childTableFieldName)
  {
    pvParentTableFields.append(parentTableFieldName);
    pvChildTableFields.append(childTableFieldName);
  }

  /*! \brief Set if a index must be created for parent table
   */
  void setCreateParentIndex(bool create)
  {
    pvCreateParentIndex = create;
  }

  /*! \brief Check if a index must be created for parent table
   */
  bool createParentIndex() const
  {
    return pvCreateParentIndex;
  }

  /*! \brief Set if a index must be created for child table
   */
  void setCreateChildIndex(bool create)
  {
    pvCreateChildIndex = create;
  }

  /*! \brief Check if a index must be created for child table
   */
  bool createChildIndex() const
  {
    return pvCreateChildIndex;
  }

  /*! \brief Set on delete action
   */
  void setOnDeleteAction(Action action)
  {
    pvOnDeleteAction = action;
  }

  /*! \brief Get on delete action
   */
  Action onDeleteAction() const
  {
    return pvOnDeleteAction;
  }

  /*! \brief Set on update action
   */
  void setOnUpdateAction(Action action)
  {
    pvOnUpdateAction = action;
  }

  /*! \brief Get on update action
   */
  Action onUpdateAction() const
  {
    return pvOnUpdateAction;
  }

  /*! \brief Clear
   */
  void clear();

  /*! \brief Get SQL statement for foreign key creation
   *
   * \pre db's driver must be loaded
   */
  QString getSqlForForeignKey(const QSqlDatabase & db);

  /*! \brief Get SQL statement to drop parent table index
   *
   * \pre db's driver must be loaded
   */
  QString getSqlForDropParentTableIndex(const QSqlDatabase & db, const QString & databaseName);

  /*! \brief Get SQL statement to create parent table index
   *
   * \pre db's driver must be loaded
   */
  QString getSqlForCreateParentTableIndex(const QSqlDatabase & db, const QString & databaseName);

  /*! \brief Get SQL statement to drop child table index
   *
   * \pre db's driver must be loaded
   */
  QString getSqlForDropChildTableIndex(const QSqlDatabase & db, const QString & databaseName);

  /*! \brief Get SQL statement to create child table index
   *
   * \pre db's driver must be loaded
   */
  QString getSqlForCreateChildTableIndex(const QSqlDatabase & db, const QString & databaseName);

 private:

  /*! \brief Get string version of given action
   */
  QString actionStr(Action action);

  uint pvCreateParentIndex : 1;
  uint pvCreateChildIndex : 1;
  Action pvOnDeleteAction;
  Action pvOnUpdateAction;
  QString pvParentTableName;
  QString pvChildTableName;
  QStringList pvParentTableFields;
  QStringList pvChildTableFields;
};

#endif // #ifndef MDT_SQL_FOREIGN_KEY_H
