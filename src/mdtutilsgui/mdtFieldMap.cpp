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
#include "mdtFieldMap.h"
#include <QSharedData>
#include <QSharedDataPointer>
#include <QMap>
#include <QMutableListIterator>

//#include <QDebug>


/*
 * mdtFieldMapData implementation
 */

mdtFieldMapData::mdtFieldMapData()
{
}

mdtFieldMapData::mdtFieldMapData(const mdtFieldMapData &other)
{
  int i;
  mdtFieldMapItem *item;

  if(&other != this){
    pvSourceFields = other.pvSourceFields;
    pvDestinationFields = other.pvDestinationFields;
    qDeleteAll(pvItems);
    pvItems.clear();
    for(i = 0; i < other.pvItems.size(); ++i){
      item = new mdtFieldMapItem;
      *item = *other.pvItems.at(i);
      pvItems.append(item);
    }
  }
}

mdtFieldMapData::~mdtFieldMapData()
{
  qDeleteAll(pvItems);
}


/*
 * mdtFieldMap implementation
 */

mdtFieldMap::mdtFieldMap()
{
  d = new mdtFieldMapData;
}

mdtFieldMap::mdtFieldMap(const mdtFieldMap &other)
 : d(other.d)
{
}

mdtFieldMap::~mdtFieldMap()
{
  clear();
}

void mdtFieldMap::setSourceFields(const QList<mdtFieldMapField> &fields)
{
  d->pvSourceFields = fields;
}

void mdtFieldMap::setSourceFieldsByDisplayTexts(const QStringList &displayTexts, int startFieldIndex)
{
  Q_ASSERT(startFieldIndex >= 0);

  int i;

  d->pvSourceFields.clear();
  for(i = 0; i < displayTexts.size(); ++i){
    mdtFieldMapField field;
    field.setIndex(i + startFieldIndex);
    field.setName(mdtFieldMapField::getFieldName(displayTexts.at(i), "_"));
    field.setDisplayText(displayTexts.at(i));
    field.sqlField().setType(QVariant::String);
    d->pvSourceFields.append(field);
  }
}

const QList<mdtFieldMapField> &mdtFieldMap::sourceFields() const
{
  return d->pvSourceFields;
}

const QList<mdtFieldMapField> mdtFieldMap::mappedSourceFields() const
{
  QMap<int, mdtFieldMapItem*> map;  // QMap will sort items by keys (= items indexes here)
  QList<mdtFieldMapItem*> items;
  mdtFieldMapItem *item;
  QList<mdtFieldMapField> fields;
  int i;

  // Fill the map
  for(i = 0; i < d->pvItems.size(); ++i){
    item = d->pvItems.at(i);
    Q_ASSERT(item != 0);
    map.insert(item->sourceFieldIndex(), item);
  }
  // Get the items list - will be sorted by keys, ascending
  items = map.values();
  // Build result
  for(i = 0; i < items.size(); ++i){
    item = items.at(i);
    Q_ASSERT(item != 0);
    fields.append(item->sourceField());
  }

  return fields;
}

const QList<mdtFieldMapField> mdtFieldMap::notMappedSourceFields(mdtFieldMap::FieldReference_t reference) const
{
  QList<mdtFieldMapField> notMappedFields;
  int itemCount = 0;
  mdtFieldMapField field;
  int i;

  for(i = 0; i < d->pvSourceFields.size(); ++i){
    // Check if field exists in map
    field = d->pvSourceFields.at(i);
    switch(reference){
      case ReferenceByIndex:
        itemCount = itemsAtSourceFieldIndex(field.index()).size();
        break;
      case ReferenceByName:
        itemCount = itemsAtSourceFieldName(field.name()).size();
        break;
    }
    // If found, add it to result
    if(itemCount == 0){
      notMappedFields.append(field);
    }
  }

  return notMappedFields;
}

const QStringList mdtFieldMap::sourceHeader() const
{
  QStringList header;
  QList<mdtFieldMapField> fields;
  mdtFieldMapField field;
  int i;

  fields = mappedSourceFields();
  for(i = 0; i < fields.size(); ++i){
    field = fields.at(i);
    if(field.displayText().isEmpty()){
      header.append(field.name());
    }else{
      header.append(field.displayText());
    }
  }

  return header;
}

void mdtFieldMap::setDestinationFields(const QList<mdtFieldMapField> &fields)
{
  d->pvDestinationFields = fields;
}

void mdtFieldMap::setDestinationFieldsByDisplayTexts(const QStringList &displayTexts, int startFieldIndex)
{
  Q_ASSERT(startFieldIndex >= 0);

  int i;

  d->pvDestinationFields.clear();
  for(i = 0; i < displayTexts.size(); ++i){
    mdtFieldMapField field;
    field.setIndex(i + startFieldIndex);
    field.setName(mdtFieldMapField::getFieldName(displayTexts.at(i), "_"));
    field.setDisplayText(displayTexts.at(i));
    field.sqlField().setType(QVariant::String);
    d->pvDestinationFields.append(field);
  }
}

void mdtFieldMap::updateDestinationFields(const QList<mdtFieldMapField> &fields, mdtFieldMap::FieldReference_t reference)
{
  mdtFieldMapItem *item;
  int idx;

  d->pvDestinationFields = fields;
  QMutableListIterator<mdtFieldMapItem*> it(d->pvItems);
  while(it.hasNext()){
    it.next();
    item = it.value();
    Q_ASSERT(item != 0);
    // Check if destination fields exists anymore and update
    idx = indexOfFieldInList(item->destinationField(), fields, reference);
    if(idx < 0){
      it.remove();
      delete item;
    }else{
      item->setDestinationField(fields.at(idx));
    }
  }
}

const QList<mdtFieldMapField> &mdtFieldMap::destinationFields() const
{
  return d->pvDestinationFields;
}

const QList<mdtFieldMapField> mdtFieldMap::mappedDestinationFields() const
{
  QMap<int, mdtFieldMapItem*> map;  // QMap will sort items by keys (= items indexes here)
  QList<mdtFieldMapItem*> items;
  mdtFieldMapItem *item;
  QList<mdtFieldMapField> fields;
  int i;

  // Fill the map
  for(i = 0; i < d->pvItems.size(); ++i){
    item = d->pvItems.at(i);
    Q_ASSERT(item != 0);
    map.insert(item->destinationFieldIndex(), item);
  }
  // Get the items list - will be sorted by keys, ascending
  items = map.values();
  // Build result
  for(i = 0; i < items.size(); ++i){
    item = items.at(i);
    Q_ASSERT(item != 0);
    fields.append(item->destinationField());
  }

  return fields;
}

const QList<mdtFieldMapField> mdtFieldMap::notMappedDestinationFields(mdtFieldMap::FieldReference_t reference) const
{
  QList<mdtFieldMapField> notMappedFields;
  mdtFieldMapItem *item = 0;
  mdtFieldMapField field;
  int i;

  for(i = 0; i < d->pvDestinationFields.size(); ++i){
    // Check if field exists in map
    field = d->pvDestinationFields.at(i);
    switch(reference){
      case ReferenceByIndex:
        item = itemAtDestinationFieldIndex(field.index());
        break;
      case ReferenceByName:
        item = itemAtDestinationFieldName(field.name());
        break;
    }
    // If found, add it to result
    if(item == 0){
      notMappedFields.append(field);
    }
  }

  return notMappedFields;

}

const QStringList mdtFieldMap::destinationHeader() const
{
  QStringList header;
  QList<mdtFieldMapField> fields;
  mdtFieldMapField field;
  int i;

  fields = mappedDestinationFields();
  for(i = 0; i < fields.size(); ++i){
    field = fields.at(i);
    if(field.displayText().isEmpty()){
      header.append(field.name());
    }else{
      header.append(field.displayText());
    }
  }

  return header;
}

void mdtFieldMap::addItem(mdtFieldMapItem *item)
{
  Q_ASSERT(item != 0);

  if(!d->pvItems.contains(item)){
    d->pvItems.append(item);
  }
}

void mdtFieldMap::generateMapping()
{
  int i, idx;
  mdtFieldMapField field;
  mdtFieldMapItem *item;

  qDeleteAll(d->pvItems);
  d->pvItems.clear();
  // Case of empty destination fields
  if(d->pvDestinationFields.isEmpty()){
    for(i = 0; i < d->pvSourceFields.size(); ++i){
      field = d->pvSourceFields.at(i);
      d->pvDestinationFields.append(field);
      item = new mdtFieldMapItem;
      item->setSourceField(field);
      item->setDestinationField(field);
      d->pvItems.append(item);
    }
  }else{
    // Case of existing destination fields
    for(i = 0; i < d->pvSourceFields.size(); ++i){
      field = d->pvSourceFields.at(i);
      idx = indexOfFieldInList(field, d->pvDestinationFields, ReferenceByName);
      if(idx > -1){
        item = new mdtFieldMapItem;
        item->setSourceField(field);
        item->setDestinationField(d->pvDestinationFields.at(idx));
        d->pvItems.append(item);
      }
    }
  }
}

void mdtFieldMap::removeItem(mdtFieldMapItem *item)
{
  if(d->pvItems.removeAll(item) < 1){
    return;
  }
  delete item;
}

void mdtFieldMap::removeItemAtSourceFieldIndex(int index)
{
  int i;

  for(i = 0; i < d->pvItems.size(); ++i){
    Q_ASSERT(d->pvItems.at(i) != 0);
    if(d->pvItems.at(i)->sourceFieldIndex() == index){
      removeItem(d->pvItems.at(i));
      break;
    }
  }
}

void mdtFieldMap::removeItemAtDestinationFieldIndex(int index)
{
  int i;

  for(i = 0; i < d->pvItems.size(); ++i){
    Q_ASSERT(d->pvItems.at(i) != 0);
    if(d->pvItems.at(i)->destinationFieldIndex() == index){
      removeItem(d->pvItems.at(i));
      break;
    }
  }
}

const QList<mdtFieldMapItem*> &mdtFieldMap::items() const
{
  return d->pvItems;
}

void mdtFieldMap::clear()
{
  qDeleteAll(d->pvItems);
  d->pvItems.clear();
  d->pvSourceFields.clear();
  d->pvDestinationFields.clear();
}

mdtFieldMapItem *mdtFieldMap::itemAtDestinationFieldIndex(int index) const
{
  int i;
  mdtFieldMapItem *item;

  for(i = 0; i < d->pvItems.size(); ++i){
    item = d->pvItems.at(i);
    Q_ASSERT(item != 0);
    if(item->destinationFieldIndex() == index){
      return item;
    }
  }

  return 0;
}

mdtFieldMapItem *mdtFieldMap::itemAtDestinationFieldName(const QString &name) const
{
  int i;
  mdtFieldMapItem *item;

  for(i = 0; i < d->pvItems.size(); ++i){
    item = d->pvItems.at(i);
    Q_ASSERT(item != 0);
    if(item->destinationFieldName() == name){
      return item;
    }
  }

  return 0;
}

QString mdtFieldMap::sourceFieldNameAtDestinationFieldIndex(int index) const
{
  QString name;
  mdtFieldMapItem *item;

  item = itemAtDestinationFieldIndex(index);
  if(item != 0){
    name = item->sourceFieldName();
  }

  return name;
}

QList<mdtFieldMapItem*> mdtFieldMap::itemsAtSourceFieldIndex(int index) const
{
  QList<mdtFieldMapItem*> items;
  mdtFieldMapItem *item;
  int i;

  for(i = 0; i < d->pvItems.size(); ++i){
    item = d->pvItems.at(i);
    Q_ASSERT(item != 0);
    if(item->sourceFieldIndex() == index){
      items.append(item);
    }
  }

  return items;
}

QList<mdtFieldMapItem*> mdtFieldMap::itemsAtSourceFieldName(const QString &name) const
{
  QList<mdtFieldMapItem*> items;
  mdtFieldMapItem *item;
  int i;

  for(i = 0; i < d->pvItems.size(); ++i){
    item = d->pvItems.at(i);
    Q_ASSERT(item != 0);
    if(item->sourceFieldName() == name){
      items.append(item);
    }
  }

  return items;
}

QVariant mdtFieldMap::dataForDestinationFieldIndex(const QStringList &sourceDataRow, int destinationFieldIndex) const
{
  mdtFieldMapItem *item = itemAtDestinationFieldIndex(destinationFieldIndex);
  if(item == 0){
    return QVariant();
  }

  return item->destinationData(sourceDataRow);
}

QVariant mdtFieldMap::dataForDestinationFieldIndex(const QList<QVariant> &sourceDataRow, int destinationFieldIndex) const
{
  mdtFieldMapItem *item = itemAtDestinationFieldIndex(destinationFieldIndex);
  if(item == 0){
    return QVariant();
  }
  // If source field splitting is not used, we can return data directly
  if((item->sourceFieldDataStartOffset() < 0)&&(item->sourceFieldDataEndOffset() < 0)){
    Q_ASSERT(item->sourceFieldIndex() < sourceDataRow.size());
    return sourceDataRow.at(item->sourceFieldIndex());
  }
  // We must convert data row to a string list and process it
  QStringList strSourceDataRow;
  for(int i = 0; i < sourceDataRow.size(); ++i){
    strSourceDataRow.append(sourceDataRow.at(i).toString());
  }

  return item->destinationData(strSourceDataRow);
}

QVariant mdtFieldMap::dataForDestinationFieldName(const QStringList &sourceDataRow, const QString &destinationFieldName) const
{
  mdtFieldMapItem *item = itemAtDestinationFieldName(destinationFieldName);
  if(item == 0){
    return QVariant();
  }
  return dataForDestinationFieldIndex(sourceDataRow, item->destinationFieldIndex());
}

const QList<QVariant> mdtFieldMap::destinationDataRow(const QList<QVariant> &sourceDataRow) const
{
  QList<QVariant> destinationRow;
  mdtFieldMapItem *item;
  int i;

  // Pre-alloc destination row (we edit it later)
  destinationRow.reserve(d->pvItems.size());
  for(i = 0; i < d->pvItems.size(); ++i){
    destinationRow.append(QVariant());
  }
  // Set destination data
  for(i = 0; i < d->pvItems.size(); ++i){
    item = d->pvItems.at(i);
    Q_ASSERT(item != 0);
    Q_ASSERT(item->sourceFieldIndex() < sourceDataRow.size());
    Q_ASSERT(item->destinationFieldIndex() < destinationRow.size());
    // If source field splitting is not used, we can directly store data
    if((item->sourceFieldDataStartOffset() < 0)&&(item->sourceFieldDataEndOffset() < 0)){
      destinationRow[item->destinationFieldIndex()] = sourceDataRow.at(item->sourceFieldIndex());
    }else{
      destinationRow[item->destinationFieldIndex()] = item->destinationData(sourceDataRow.at(item->sourceFieldIndex()).toString());
    }
  }

  return destinationRow;
}

const QList<QVariant> mdtFieldMap::destinationDataRow(const QStringList &sourceDataRow) const
{
  QList<QVariant> destinationRow;
  mdtFieldMapItem *item;
  int i;

  // Pre-alloc destination row (we edit it later)
  destinationRow.reserve(d->pvItems.size());
  for(i = 0; i < d->pvItems.size(); ++i){
    destinationRow.append(QVariant());
  }
  // Set destination data
  for(i = 0; i < d->pvItems.size(); ++i){
    item = d->pvItems.at(i);
    Q_ASSERT(item != 0);
    Q_ASSERT(item->sourceFieldIndex() < sourceDataRow.size());
    Q_ASSERT(item->destinationFieldIndex() < destinationRow.size());
    // If source field splitting is not used, we can directly store data
    if((item->sourceFieldDataStartOffset() < 0)&&(item->sourceFieldDataEndOffset() < 0)){
      destinationRow[item->destinationFieldIndex()] = sourceDataRow.at(item->sourceFieldIndex());
    }else{
      destinationRow[item->destinationFieldIndex()] = item->destinationData(sourceDataRow.at(item->sourceFieldIndex()));
    }
  }

  return destinationRow;
}

QHash<QString, QString> mdtFieldMap::displayTextsByFieldNames() const
{
  QHash<QString, QString> result;
  mdtFieldMapItem *item;
  int i;

  for(i=0; i < d->pvItems.size(); ++i){
    item = d->pvItems.at(i);
    Q_ASSERT(item != 0);
    result.insert(item->destinationFieldName(), item->destinationFieldDisplayText());
  }

  return result;
}

void mdtFieldMap::insertDataIntoSourceString(QString &str, const QVariant &data, mdtFieldMapItem *item)
{
  Q_ASSERT(item != 0);

  int start, end;
  QString strData = data.toString();

  if((item->sourceFieldDataStartOffset()<0)&&(item->sourceFieldDataEndOffset()<0)){
    str = strData;
    return;
  }
  start = item->sourceFieldDataStartOffset();
  if(start<0){
    start = 0;
  }
  end = item->sourceFieldDataEndOffset();
  if(end<0){
    end = start + strData.size()-1;
  }
  if(str.size() < (end+1)){
    str.resize(end+1);
  }
  str.replace(start, end-start+1, strData);
}

bool mdtFieldMap::fieldExistsInList(const mdtFieldMapField field, const QList<mdtFieldMapField> &list, mdtFieldMap::FieldReference_t reference) const
{
  return (indexOfFieldInList(field, list, reference) > -1);
}

int mdtFieldMap::indexOfFieldInList(const mdtFieldMapField field, const QList<mdtFieldMapField> &list, mdtFieldMap::FieldReference_t reference) const
{
  switch(reference){
    case ReferenceByIndex:
      return indexOfFieldInListByFieldIndex(field, list);
    case ReferenceByName:
      return indexOfFieldInListByFieldName(field, list);
  }

  return -1;
}

int mdtFieldMap::indexOfFieldInListByFieldIndex(const mdtFieldMapField field, const QList<mdtFieldMapField> &list) const
{
  int i;

  for(i = 0; i < list.size(); ++i){
    if(list.at(i).index() == field.index()){
      return i;
    }
  }

  return -1;
}

int mdtFieldMap::indexOfFieldInListByFieldName(const mdtFieldMapField field, const QList<mdtFieldMapField> &list) const
{
  int i;

  for(i = 0; i < list.size(); ++i){
    if(list.at(i).name() == field.name()){
      return i;
    }
  }

  return -1;
}
