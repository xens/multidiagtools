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
#ifndef MDT_SQL_RELATION_ITEM_H
#define MDT_SQL_RELATION_ITEM_H

#include <QSqlField>
#include <QString>
#include <QVariant>

/*! \brief Keep informations about a parent/child SQL relation
 */
class mdtSqlRelationItem
{
 public:

  /*! \brief Build a empty relation item
   */
  mdtSqlRelationItem();

  /*! \brief Destructor
   */
  ~mdtSqlRelationItem();

  /*! \brief Set parent field
   */
  void setParentField(const QSqlField &field);

  /*! \brief Set parent field index
   */
  void setParentFieldIndex(int index);

  /*! \brief Set child field
   */
  void setChildField(const QSqlField &field);

  /*! \brief Set child field index
   */
  void setChildFieldIndex(int index);

  /*! \brief Get parent field name
   */
  inline QString parentFieldName() const { return pvParentField.name(); }

  /*! \brief Get parent field index
   */
  inline int parentFieldIndex() const { return pvParentFieldIndex; }

  /*! \brief Get child field name
   */
  inline QString childFieldName() const { return pvChildField.name(); }

  /*! \brief Get child field index
   */
  inline int childFieldIndex() const { return pvChildFieldIndex; }

  /*! \brief Get table/field protection
   *
   * Typical name protection is `
   */
  inline QString nameProtection() const { return "`"; }

  /*! \brief Get data protection
   *
   * Typical data protections are:
   *  - ' for string type
   *  - nothing for numeric type
   */
  inline QString dataProtection() const { return pvDataProtection; }

  /*! \brief Get data type
   */
  inline QVariant::Type dataType() const { return pvParentField.type(); }

  /*! \brief Get value for a filter that gives no data
   *
   * Typical values are:
   *  - empty string
   *  - -1 for numeric field
   */
  inline QVariant valueForNoDataFilter() const { return pvValueForNoDataFilter; }

  /*! \brief Set copyParentToChildOnInsertion flag
   *
   * Usually, a parent/child relation is based on a primary key
   *  in parent table and foreing key (that refers to parent PK)
   *  in child model. When inserting a new record in child table,
   *  parent table's primary key must be copied into foreing key
   *  fields in child table.
   *  Set this flag to true to enable this behaviour (see mdtSqlRelation for more details).
   *
   * By default, this flag is false.
   */
  void setCopyParentToChildOnInsertion(bool enable);

  /*! \brief Set copyParentToChildOnInsertion flag
   *
   * \sa setCopyParentToChildOnInsertion()
   */
  inline bool copyParentToChildOnInsertion() const { return pvCopyParentToChildOnInsertion; }

  /*! \brief Set relation operator with previous item
   *
   * For most of cases, the relation operator between items is AND,
   *  that is the default.
   *  For some cases, it can be usefull to have a OR operator between 2 relation items.
   *  On this case, specify this here.
   */
  void setRelationOperatorWithPreviousItem(const QString &operatorWithPreviousItem);

  /*! \brief Get relation operator with previous item
   *
   * \sa setRelationOperatorWithPreviousItem()
   */
  inline QString relationOperatorWithPreviousItem() const { return pvRelationOperatorWithPreviousItem; }

 private:

  Q_DISABLE_COPY(mdtSqlRelationItem);

  QSqlField pvParentField;
  int pvParentFieldIndex;
  QSqlField pvChildField;
  int pvChildFieldIndex;
  QString pvNameProtection;
  QString pvDataProtection;
  QVariant pvValueForNoDataFilter;
  bool pvCopyParentToChildOnInsertion;
  QString pvRelationOperatorWithPreviousItem;
};

#endif  // #ifndef MDT_SQL_RELATION_ITEM_H
