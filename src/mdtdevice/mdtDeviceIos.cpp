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
#include "mdtDeviceIos.h"
#include "mdtDeviceIosSegment.h"
#include <QMapIterator>
#include <QMutableListIterator>
#include <QtGlobal>

#include <QDebug>

mdtDeviceIos::mdtDeviceIos(QObject *parent)
 : QObject(parent)
{
  pvAutoDeleteIos = true;
}

mdtDeviceIos::~mdtDeviceIos()
{
  // Delete I/O objects
  qDebug() << "mdtDeviceIos::~mdtDeviceIos() ...";
  if(pvAutoDeleteIos){
    deleteIos();
  }else{
    pvAnalogInputs.clear();
    pvAnalogInputsByAddressRead.clear();
    qDeleteAll(pvAnalogInputsSegments);
    pvAnalogInputsSegments.clear();
    pvAnalogOutputs.clear();
    pvAnalogOutputsByAddressRead.clear();
    pvAnalogOutputsByAddressWrite.clear();
    qDeleteAll(pvAnalogOutputsSegments);
    pvAnalogOutputsSegments.clear();
    pvDigitalInputs.clear();
    pvDigitalInputsByAddressRead.clear();
    qDeleteAll(pvDigitalInputsSegments);
    pvDigitalInputsSegments.clear();
    pvDigitalOutputs.clear();
    pvDigitalOutputsByAddressRead.clear();
    pvDigitalOutputsByAddressWrite.clear();
    qDeleteAll(pvDigitalOutputsSegments);
    pvDigitalOutputsSegments.clear();
  }

  qDebug() << "mdtDeviceIos::~mdtDeviceIos() DONE";
}

void mdtDeviceIos::setAutoDeleteIos(bool autoDelete)
{
  pvAutoDeleteIos = autoDelete;
}

void mdtDeviceIos::deleteIos()
{
  qDebug() << "delete I/Os ...";
  qDeleteAll(pvAnalogInputs);
  pvAnalogInputs.clear();
  pvAnalogInputsByAddressRead.clear();
  qDeleteAll(pvAnalogInputsSegments);
  pvAnalogInputsSegments.clear();
  qDeleteAll(pvAnalogOutputs);
  pvAnalogOutputs.clear();
  pvAnalogOutputsByAddressRead.clear();
  pvAnalogOutputsByAddressWrite.clear();
  qDeleteAll(pvAnalogOutputsSegments);
  pvAnalogOutputsSegments.clear();
  qDeleteAll(pvDigitalInputs);
  pvDigitalInputs.clear();
  pvDigitalInputsByAddressRead.clear();
  qDeleteAll(pvDigitalInputsSegments);
  pvDigitalInputsSegments.clear();
  qDeleteAll(pvDigitalOutputs);
  pvDigitalOutputs.clear();
  pvDigitalOutputsByAddressRead.clear();
  pvDigitalOutputsByAddressWrite.clear();
  qDeleteAll(pvDigitalOutputsSegments);
  pvDigitalOutputsSegments.clear();
  qDebug() << "delete I/Os DONE";
}

void mdtDeviceIos::addAnalogInput(mdtAnalogIo *ai)
{
  Q_ASSERT(ai != 0);

  QList<mdtAnalogIo*> aiList;
  mdtDeviceIosSegment *segment;
  QList<mdtAnalogIo*> segmentAiList;
  int i;

  // Check that I/O was not allready set and add it
  Q_ASSERT(!pvAnalogInputs.contains(ai));
  Q_ASSERT(!pvAnalogInputsByAddressRead.contains(ai->addressRead()));
  pvAnalogInputs.append(ai);
  pvAnalogInputsByAddressRead.insert(ai->addressRead(), ai);
  Q_ASSERT(pvAnalogInputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvAnalogInputsByAddressRead.values().at(0) != 0);
  // We must reorganize segments - We use the QMap container, because it is allready sorted by keys, ascending
  qDeleteAll(pvAnalogInputsSegments);
  pvAnalogInputsSegments.clear();
  aiList = pvAnalogInputsByAddressRead.values();
  Q_ASSERT(aiList.size() > 0);
  // We know that we have at least 1 I/O - Just create a segment and add first item
  segment = new mdtDeviceIosSegment;
  segmentAiList.append(aiList.at(0));
  for(i = 1; i < aiList.size(); ++i){
    Q_ASSERT(aiList.at(i) != 0);
    // If address of current item is not directly successor of previous, we have a hole -> create a new segment
    if((aiList.at(i)->addressRead() - aiList.at(i-1)->addressRead() ) != 1){
      // Add current segment to list and create a new one
      segment->setIos(segmentAiList);
      segmentAiList.clear();
      pvAnalogInputsSegments.append(segment);
      segment = new mdtDeviceIosSegment;
    }
    segmentAiList.append(aiList.at(i));
  }
  // Add current segment to list
  segment->setIos(segmentAiList);
  pvAnalogInputsSegments.append(segment);
}

void mdtDeviceIos::addAnalogInputs(const QList<mdtAnalogIo*> & aiList)
{
  int i;

  for(i = 0; i < aiList.size(); ++i){
    Q_ASSERT(aiList.at(i) != 0);
    addAnalogInput(aiList.at(i));
  }
}

mdtAnalogIo *mdtDeviceIos::analogInputAtAddress(int address)
{
  return pvAnalogInputsByAddressRead.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogInputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvAnalogInputs.size(); ++i){
    Q_ASSERT(pvAnalogInputs.at(i) != 0);
    if(pvAnalogInputs.at(i)->labelShort() == labelShort){
      return pvAnalogInputs.at(i);
    }
  }

  return 0;
}

const QList<mdtAnalogIo*> mdtDeviceIos::analogInputs() const
{
  return pvAnalogInputs;
}

const QList<mdtDeviceIosSegment*> &mdtDeviceIos::analogInputsSegments() const
{
  return pvAnalogInputsSegments;
}

int mdtDeviceIos::analogInputsCount() const
{
  return pvAnalogInputs.size();
}

void mdtDeviceIos::setAnalogInputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvAnalogInputs.size(); ++i){
    Q_ASSERT(pvAnalogInputs.at(i) != 0);
    pvAnalogInputs.at(i)->setValue(value);
  }
}

void mdtDeviceIos::addAnalogOutput(mdtAnalogIo *ao, mdtDeviceIos::addressAccess_t sortSegmentsBy)
{
  Q_ASSERT(ao != 0);

  QList<mdtAnalogIo*> aoList;
  mdtDeviceIosSegment *segment;
  QList<mdtAnalogIo*> segmentAoList;
  int i;
  int currentAddress, previousAddress;

  // Check if I/O was allready set and add it
  Q_ASSERT(!pvAnalogOutputs.contains(ao));
  Q_ASSERT(!pvAnalogOutputsByAddressRead.contains(ao->addressRead()));
  Q_ASSERT(!pvAnalogOutputsByAddressWrite.contains(ao->addressWrite()));
  pvAnalogOutputs.append(ao);
  pvAnalogOutputsByAddressRead.insert(ao->addressRead(), ao);
  pvAnalogOutputsByAddressWrite.insert(ao->addressWrite(), ao);
  // Get first addresses for read and write access
  Q_ASSERT(pvAnalogOutputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvAnalogOutputsByAddressRead.values().at(0) != 0);
  Q_ASSERT(pvAnalogOutputsByAddressWrite.values().size() > 0);
  Q_ASSERT(pvAnalogOutputsByAddressWrite.values().at(0) != 0);
  // We must reorganize segments - We use the QMap container, because it is allready sorted by keys, ascending
  qDeleteAll(pvAnalogOutputsSegments);
  pvAnalogOutputsSegments.clear();
  if(sortSegmentsBy == Read){
    aoList = pvAnalogOutputsByAddressRead.values();
  }else{
    aoList = pvAnalogOutputsByAddressWrite.values();
  }
  Q_ASSERT(aoList.size() > 0);
  // We know that we have at least 1 I/O - Just create a segment and add first item
  segment = new mdtDeviceIosSegment;
  segmentAoList.append(aoList.at(0));
  for(i = 1; i < aoList.size(); ++i){
    Q_ASSERT(aoList.at(i) != 0);
    // If address of current item is not directly successor of previous, we have a hole -> create a new segment
    if(sortSegmentsBy == Read){
      currentAddress = aoList.at(i)->addressRead();
      previousAddress = aoList.at(i-1)->addressRead();
    }else{
      currentAddress = aoList.at(i)->addressWrite();
      previousAddress = aoList.at(i-1)->addressWrite();
    }
    if((currentAddress - previousAddress) != 1){
      // Add current segment to list and create a new one
      segment->setIos(segmentAoList);
      segmentAoList.clear();
      pvAnalogOutputsSegments.append(segment);
      segment = new mdtDeviceIosSegment;
    }
    segmentAoList.append(aoList.at(i));
  }
  // Add current segment to list
  segment->setIos(segmentAoList);
  pvAnalogOutputsSegments.append(segment);
}

void mdtDeviceIos::addAnalogOutputs(const QList<mdtAnalogIo*> & aoList, addressAccess_t sortSegmentsBy)
{
  int i;

  for(i = 0; i < aoList.size(); ++i){
    Q_ASSERT(aoList.at(i) != 0);
    addAnalogOutput(aoList.at(i), sortSegmentsBy);
  }
}

mdtAnalogIo *mdtDeviceIos::analogOutputAtAddressRead(int address)
{
  return pvAnalogOutputsByAddressRead.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogOutputAtAddressWrite(int address)
{
  return pvAnalogOutputsByAddressWrite.value(address, 0);
}

mdtAnalogIo *mdtDeviceIos::analogOutputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    if(pvAnalogOutputs.at(i)->labelShort() == labelShort){
      return pvAnalogOutputs.at(i);
    }
  }

  return 0;
}

const QList<mdtAnalogIo*> mdtDeviceIos::analogOutputs() const
{
  return pvAnalogOutputs;
}

const QList<mdtDeviceIosSegment*> &mdtDeviceIos::analogOutputsSegments() const
{
  return pvAnalogOutputsSegments;
}

int mdtDeviceIos::analogOutputsCount() const
{
  return pvAnalogOutputs.size();
}

QList<int> mdtDeviceIos::analogOutputsValuesIntByAddressWrite() const
{
  QList<int> values;
  QList<mdtAnalogIo*> aos;
  int i;

  aos = pvAnalogOutputsByAddressWrite.values();
  for(i=0; i<aos.size(); i++){
    Q_ASSERT(aos.at(i) != 0);
    values.append(aos.at(i)->value().valueInt());
  }

  return values;
}

void mdtDeviceIos::setAnalogOutputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    pvAnalogOutputs.at(i)->setValue(value, false);
  }
}

void mdtDeviceIos::addDigitalInput(mdtDigitalIo *di)
{
  Q_ASSERT(di != 0);

  QList<mdtDigitalIo*> diList;
  mdtDeviceIosSegment *segment;
  QList<mdtDigitalIo*> segmentDiList;
  int i;

  // Check that I/O was not allready set and add it
  Q_ASSERT(!pvDigitalInputs.contains(di));
  Q_ASSERT(!pvDigitalInputsByAddressRead.contains(di->addressRead()));
  pvDigitalInputs.append(di);
  pvDigitalInputsByAddressRead.insert(di->address(), di);
  // QMap returns a list sorted by keys, ascending
  Q_ASSERT(pvDigitalInputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvDigitalInputsByAddressRead.values().at(0) != 0);

  // We must reorganize segments - We use the QMap container, because it is allready sorted by keys, ascending
  qDeleteAll(pvDigitalInputsSegments);
  pvDigitalInputsSegments.clear();
  diList = pvDigitalInputsByAddressRead.values();
  Q_ASSERT(diList.size() > 0);
  // We know that we have at least 1 I/O - Just create a segment and add first item
  segment = new mdtDeviceIosSegment;
  segmentDiList.append(diList.at(0));
  for(i = 1; i < diList.size(); ++i){
    Q_ASSERT(diList.at(i) != 0);
    // If address of current item is not directly successor of previous, we have a hole -> create a new segment
    if((diList.at(i)->addressRead() - diList.at(i-1)->addressRead() ) != 1){
      // Add current segment to list and create a new one
      segment->setIos(segmentDiList);
      segmentDiList.clear();
      pvDigitalInputsSegments.append(segment);
      segment = new mdtDeviceIosSegment;
    }
    segmentDiList.append(diList.at(i));
  }
  // Add current segment to list
  segment->setIos(segmentDiList);
  pvDigitalInputsSegments.append(segment);
}

void mdtDeviceIos::addDigitalInputs(const QList<mdtDigitalIo*> & diList)
{
  int i;

  for(i = 0; i < diList.size(); ++i){
    Q_ASSERT(diList.at(i) != 0);
    addDigitalInput(diList.at(i));
  }
}

mdtDigitalIo *mdtDeviceIos::digitalInputAtAddress(int address)
{
  return pvDigitalInputsByAddressRead.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalInputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvDigitalInputs.size(); ++i){
    Q_ASSERT(pvDigitalInputs.at(i) != 0);
    if(pvDigitalInputs.at(i)->labelShort() == labelShort){
      return pvDigitalInputs.at(i);
    }
  }

  return 0;
}

const QList<mdtDigitalIo*> mdtDeviceIos::digitalInputs() const
{
  return pvDigitalInputs;
}

int mdtDeviceIos::digitalInputsCount() const
{
  return pvDigitalInputs.size();
}

const QList<mdtDeviceIosSegment*> &mdtDeviceIos::digitalInputsSegments() const
{
  return pvDigitalInputsSegments;
}

void mdtDeviceIos::setDigitalInputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvDigitalInputs.size(); ++i){
    Q_ASSERT(pvDigitalInputs.at(i) != 0);
    pvDigitalInputs.at(i)->setValue(value);
  }
}

void mdtDeviceIos::addDigitalOutput(mdtDigitalIo *dout, addressAccess_t sortSegmentsBy)
{
  Q_ASSERT(dout != 0);

  QList<mdtDigitalIo*> doList;
  mdtDeviceIosSegment *segment;
  QList<mdtDigitalIo*> segmentDoList;
  int i;
  int currentAddress, previousAddress;

  // Check if I/O was allready set and add it
  Q_ASSERT(!pvDigitalOutputs.contains(dout));
  Q_ASSERT(!pvDigitalOutputsByAddressRead.contains(dout->addressRead()));
  Q_ASSERT(!pvDigitalOutputsByAddressWrite.contains(dout->addressWrite()));
  pvDigitalOutputs.append(dout);
  pvDigitalOutputsByAddressRead.insert(dout->addressRead(), dout);
  pvDigitalOutputsByAddressWrite.insert(dout->addressWrite(), dout);
  Q_ASSERT(pvDigitalOutputsByAddressRead.values().size() > 0);
  Q_ASSERT(pvDigitalOutputsByAddressRead.values().at(0) != 0);
  Q_ASSERT(pvDigitalOutputsByAddressWrite.values().size() > 0);
  Q_ASSERT(pvDigitalOutputsByAddressWrite.values().at(0) != 0);
  // We must reorganize segments - We use the QMap container, because it is allready sorted by keys, ascending
  qDeleteAll(pvDigitalOutputsSegments);
  pvDigitalOutputsSegments.clear();
  if(sortSegmentsBy == Read){
    doList = pvDigitalOutputsByAddressRead.values();
  }else{
    doList = pvDigitalOutputsByAddressWrite.values();
  }
  Q_ASSERT(doList.size() > 0);
  // We know that we have at least 1 I/O - Just create a segment and add first item
  segment = new mdtDeviceIosSegment;
  segmentDoList.append(doList.at(0));
  for(i = 1; i < doList.size(); ++i){
    Q_ASSERT(doList.at(i) != 0);
    // If address of current item is not directly successor of previous, we have a hole -> create a new segment
    if(sortSegmentsBy == Read){
      currentAddress = doList.at(i)->addressRead();
      previousAddress = doList.at(i-1)->addressRead();
    }else{
      currentAddress = doList.at(i)->addressWrite();
      previousAddress = doList.at(i-1)->addressWrite();
    }
    if((currentAddress - previousAddress) != 1){
      // Add current segment to list and create a new one
      segment->setIos(segmentDoList);
      segmentDoList.clear();
      pvDigitalOutputsSegments.append(segment);
      segment = new mdtDeviceIosSegment;
    }
    segmentDoList.append(doList.at(i));
  }
  // Add current segment to list
  segment->setIos(segmentDoList);
  pvDigitalOutputsSegments.append(segment);
}

void mdtDeviceIos::addDigitalOutputs(const QList<mdtDigitalIo*> & doList, addressAccess_t sortSegmentsBy)
{
  int i;

  for(i = 0; i < doList.size(); ++i){
    Q_ASSERT(doList.at(i) != 0);
    addDigitalOutput(doList.at(i), sortSegmentsBy);
  }
}

mdtDigitalIo *mdtDeviceIos::digitalOutputAtAddressRead(int address)
{
  return pvDigitalOutputsByAddressRead.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalOutputAtAddressWrite(int address)
{
  return pvDigitalOutputsByAddressWrite.value(address, 0);
}

mdtDigitalIo *mdtDeviceIos::digitalOutputWithLabelShort(const QString &labelShort)
{
  int i;

  for(i=0; i<pvDigitalOutputs.size(); ++i){
    Q_ASSERT(pvDigitalOutputs.at(i) != 0);
    if(pvDigitalOutputs.at(i)->labelShort() == labelShort){
      return pvDigitalOutputs.at(i);
    }
  }

  return 0;
}

const QList<mdtDigitalIo*> mdtDeviceIos::digitalOutputs() const
{
  return pvDigitalOutputs;
}

const QList<mdtDeviceIosSegment*> &mdtDeviceIos::digitalOutputsSegments() const
{
  return pvDigitalOutputsSegments;
}

int mdtDeviceIos::digitalOutputsCount() const
{
  return pvDigitalOutputs.size();
}

void mdtDeviceIos::setDigitalOutputsValue(const mdtValue &value)
{
  int i;

  for(i=0; i<pvDigitalOutputs.size(); ++i){
    Q_ASSERT(pvDigitalOutputs.at(i) != 0);
    pvDigitalOutputs.at(i)->setValue(value);
  }
}

QList<bool> mdtDeviceIos::digitalOutputsStatesByAddressWrite() const
{
  QList<bool> states;
  QList<mdtDigitalIo*> dos;
  int i;

  dos = pvDigitalOutputsByAddressWrite.values();
  for(i=0; i<dos.size(); i++){
    Q_ASSERT(dos.at(i) != 0);
    states.append(dos.at(i)->value().valueBool());
  }

  return states;
}

void mdtDeviceIos::setIosDefaultLabelShort()
{
  QList<mdtAnalogIo*> aiList;
  QList<mdtAnalogIo*> aoList;
  QList<mdtDigitalIo*> diList;
  QList<mdtDigitalIo*> doList;
  int i;

  // We get I/O lists from QMap containers, because they are sorted by addresses, ascending
  aiList = pvAnalogInputsByAddressRead.values();
  aoList = pvAnalogOutputsByAddressRead.values();
  diList = pvDigitalInputsByAddressRead.values();
  doList = pvDigitalOutputsByAddressRead.values();

  for(i = 0; i < aiList.size(); ++i){
    Q_ASSERT(aiList.at(i) != 0);
    aiList.at(i)->setLabelShort("AI" + QString::number(i+1));
  }
  for(i = 0; i < aoList.size(); ++i){
    Q_ASSERT(aoList.at(i) != 0);
    aoList.at(i)->setLabelShort("AO" + QString::number(i+1));
  }
  for(i = 0; i < diList.size(); ++i){
    Q_ASSERT(diList.at(i) != 0);
    diList.at(i)->setLabelShort("DI" + QString::number(i+1));
  }
  for(i = 0; i < doList.size(); ++i){
    Q_ASSERT(doList.at(i) != 0);
    doList.at(i)->setLabelShort("DO" + QString::number(i+1));
  }
}

void mdtDeviceIos::updateAnalogInputValues(const QList<QVariant> &values, const int firstAddress, const int n, bool matchAddresses)
{
  int i, max, cursor, stored;
  mdtDeviceIosSegment *segment;
  QList<mdtDeviceIosSegment*> segmentList;
  int segmentSpace;
  bool found = false;
  int address;

  // Find the first segment
  i = 0;
  if(analogInputsSegments().isEmpty()){
    return;
  }
  if(firstAddress < 0){
    segment = analogInputsSegments().at(0);
    Q_ASSERT(segment != 0);
    address = segment->startAddressRead();
    found = true;
  }else{
    address = firstAddress;
    for(i = 0; i < analogInputsSegments().size(); ++i){
      segment = analogInputsSegments().at(i);
      Q_ASSERT(segment != 0);
      if(segment->containsAddressRead(firstAddress)){
        found = true;
        break;
      }
    }
  }
  if(!found){
    return;
  }
  Q_ASSERT(segment != 0);
  segmentSpace = segment->ioCount();
  segmentList.append(segment);
  // Fix quantity of inputs to update
  if(n < 0){
    max = qMin(values.size(), pvAnalogInputs.size());
  }else{
    max = qMin(n, pvAnalogInputs.size());
  }
  Q_ASSERT(max <= values.size());
  // Find needed amount of segments to store max items
  for(i = 1; i < analogInputsSegments().size(); ++i){
    segment = analogInputsSegments().at(i);
    Q_ASSERT(segment != 0);
    segmentSpace += segment->ioCount();
    segmentList.append(segment);
    if(segmentSpace >= max){
      break;
    }
  }
  // Update
  cursor = 0;
  Q_ASSERT(segmentList.size() > 0);
  segment = segmentList.at(0);
  i = 0;
  while(cursor < max){
    // Store in current segment
    stored = segment->updateValuesFromAddressRead(address, values.mid(cursor), max);
    // Get next segment - If no more available, we have finished
    ++i;
    if(i < segmentList.size()){
      segment = segmentList.at(i);
    }else{
      return;
    }
    Q_ASSERT(segment != 0);
    // Update data cursor and address
    cursor += stored;
    address += stored;
    // If matchAddresses is true , we must matter about address match
    if(matchAddresses){
      while((cursor < max)&&(!segment->containsAddressRead(address))){
        ++address;
        ++cursor;
      }
    }else{
      address = segment->startAddressRead();
    }
  }
}

void mdtDeviceIos::updateAnalogOutputValues(const QList<QVariant> &values, int firstAddress, addressAccess_t firstAddressAccess, int n, bool matchAddresses)
{
  int i, max, cursor, stored;
  mdtDeviceIosSegment *segment;
  QList<mdtDeviceIosSegment*> segmentList;
  int segmentSpace;
  bool found = false;
  int address;

  // Find the first segment
  i = 0;
  if(analogOutputsSegments().isEmpty()){
    return;
  }
  if(firstAddress < 0){
    segment = analogOutputsSegments().at(0);
    Q_ASSERT(segment != 0);
    if(firstAddressAccess == Read){
      address = segment->startAddressRead();
    }else{
      address = segment->startAddressWrite();
    }
    found = true;
  }else{
    address = firstAddress;
    for(i = 0; i < analogOutputsSegments().size(); ++i){
      segment = analogOutputsSegments().at(i);
      Q_ASSERT(segment != 0);
      if(firstAddressAccess == Read){
        found = segment->containsAddressRead(firstAddress);
      }else{
        found = segment->containsAddressWrite(firstAddress);
      }
      if(found){
        break;
      }
    }
  }
  if(!found){
    return;
  }
  Q_ASSERT(segment != 0);
  segmentSpace = segment->ioCount();
  segmentList.append(segment);
  // Fix quantity of inputs to update
  if(n < 0){
    max = qMin(values.size(), pvAnalogOutputs.size());
  }else{
    max = qMin(n, pvAnalogOutputs.size());
  }
  Q_ASSERT(max <= values.size());
  // Find needed amount of segments to store max items
  for(i = 1; i < analogOutputsSegments().size(); ++i){
    segment = analogOutputsSegments().at(i);
    Q_ASSERT(segment != 0);
    segmentSpace += segment->ioCount();
    segmentList.append(segment);
    if(segmentSpace >= max){
      break;
    }
  }
  // Update
  cursor = 0;
  Q_ASSERT(segmentList.size() > 0);
  segment = segmentList.at(0);
  i = 0;
  while(cursor < max){
    // Store in current segment
    if(firstAddressAccess == Read){
      stored = segment->updateValuesFromAddressRead(address, values.mid(cursor), max);
    }else{
      stored = segment->updateValuesFromAddressWrite(address, values.mid(cursor), max);
    }
    // Get next segment - If no more available, we have finished
    ++i;
    if(i < segmentList.size()){
      segment = segmentList.at(i);
    }else{
      return;
    }
    Q_ASSERT(segment != 0);
    // Update data cursor and address
    cursor += stored;
    address += stored;
    // If matchAddresses is true , we must matter about address match
    if(matchAddresses){
      if(firstAddressAccess == Read){
        while((cursor < max)&&(!segment->containsAddressRead(address))){
          ++address;
          ++cursor;
        }
      }else{
        while((cursor < max)&&(!segment->containsAddressWrite(address))){
          ++address;
          ++cursor;
        }
      }
    }else{
      if(firstAddressAccess == Read){
        address = segment->startAddressRead();
      }else{
        address = segment->startAddressWrite();
      }
    }
  }
}

void mdtDeviceIos::setAnalogOutputsEnabled(bool enabled)
{
  int i;

  for(i=0; i<pvAnalogOutputs.size(); ++i){
    Q_ASSERT(pvAnalogOutputs.at(i) != 0);
    pvAnalogOutputs.at(i)->setEnabled(enabled);
  }
}

void mdtDeviceIos::updateDigitalInputValues(const QList<QVariant> &values, int firstAddress, int n, bool matchAddresses)
{
  int i, max, cursor, stored;
  mdtDeviceIosSegment *segment;
  QList<mdtDeviceIosSegment*> segmentList;
  int segmentSpace;
  bool found = false;
  int address;

  // Find the first segment
  i = 0;
  if(digitalInputsSegments().isEmpty()){
    return;
  }
  if(firstAddress < 0){
    segment = digitalInputsSegments().at(0);
    Q_ASSERT(segment != 0);
    address = segment->startAddressRead();
    found = true;
  }else{
    address = firstAddress;
    for(i = 0; i < digitalInputsSegments().size(); ++i){
      segment = digitalInputsSegments().at(i);
      Q_ASSERT(segment != 0);
      if(segment->containsAddressRead(firstAddress)){
        found = true;
        break;
      }
    }
  }
  if(!found){
    return;
  }
  Q_ASSERT(segment != 0);
  segmentSpace = segment->ioCount();
  segmentList.append(segment);
  // Fix quantity of inputs to update
  if(n < 0){
    max = qMin(values.size(), pvDigitalInputs.size());
  }else{
    max = qMin(n, pvDigitalInputs.size());
  }
  Q_ASSERT(max <= values.size());
  // Find needed amount of segments to store max items
  for(i = 1; i < digitalInputsSegments().size(); ++i){
    segment = digitalInputsSegments().at(i);
    Q_ASSERT(segment != 0);
    segmentSpace += segment->ioCount();
    segmentList.append(segment);
    if(segmentSpace >= max){
      break;
    }
  }
  // Update
  cursor = 0;
  Q_ASSERT(segmentList.size() > 0);
  segment = segmentList.at(0);
  i = 0;
  while(cursor < max){
    // Store in current segment
    stored = segment->updateValuesFromAddressRead(address, values.mid(cursor), max);
    // Get next segment - If no more available, we have finished
    ++i;
    if(i < segmentList.size()){
      segment = segmentList.at(i);
    }else{
      return;
    }
    Q_ASSERT(segment != 0);
    // Update data cursor and address
    cursor += stored;
    address += stored;
    // If matchAddresses is true , we must matter about address match
    if(matchAddresses){
      while((cursor < max)&&(!segment->containsAddressRead(address))){
        ++address;
        ++cursor;
      }
    }else{
      address = segment->startAddressRead();
    }
  }
}

void mdtDeviceIos::updateDigitalOutputValues(const QList<QVariant> &values, int firstAddress, addressAccess_t firstAddressAccess, int n, bool matchAddresses)
{
  int i, max, cursor, stored;
  mdtDeviceIosSegment *segment;
  QList<mdtDeviceIosSegment*> segmentList;
  int segmentSpace;
  bool found = false;
  int address;

  // Find the first segment
  i = 0;
  if(digitalOutputsSegments().isEmpty()){
    return;
  }
  if(firstAddress < 0){
    segment = digitalOutputsSegments().at(0);
    Q_ASSERT(segment != 0);
    if(firstAddressAccess == Read){
      address = segment->startAddressRead();
    }else{
      address = segment->startAddressWrite();
    }
    found = true;
  }else{
    address = firstAddress;
    for(i = 0; i < digitalOutputsSegments().size(); ++i){
      segment = digitalOutputsSegments().at(i);
      Q_ASSERT(segment != 0);
      if(firstAddressAccess == Read){
        found = segment->containsAddressRead(firstAddress);
      }else{
        found = segment->containsAddressWrite(firstAddress);
      }
      if(found){
        break;
      }
    }
  }
  if(!found){
    return;
  }
  Q_ASSERT(segment != 0);
  segmentSpace = segment->ioCount();
  segmentList.append(segment);
  // Fix quantity of outputs to update
  if(n < 0){
    max = qMin(values.size(), pvDigitalOutputs.size());
  }else{
    max = qMin(n, pvDigitalOutputs.size());
  }
  Q_ASSERT(max <= values.size());
  // Find needed amount of segments to store max items
  for(i = 1; i < digitalOutputsSegments().size(); ++i){
    segment = digitalOutputsSegments().at(i);
    Q_ASSERT(segment != 0);
    segmentSpace += segment->ioCount();
    segmentList.append(segment);
    if(segmentSpace >= max){
      break;
    }
  }
  // Update
  cursor = 0;
  Q_ASSERT(segmentList.size() > 0);
  segment = segmentList.at(0);
  i = 0;
  while(cursor < max){
    // Store in current segment
    if(firstAddressAccess == Read){
      stored = segment->updateValuesFromAddressRead(address, values.mid(cursor), max);
    }else{
      stored = segment->updateValuesFromAddressWrite(address, values.mid(cursor), max);
    }
    // Get next segment - If no more available, we have finished
    ++i;
    if(i < segmentList.size()){
      segment = segmentList.at(i);
    }else{
      return;
    }
    Q_ASSERT(segment != 0);
    // Update data cursor and address
    cursor += stored;
    address += stored;
    // If matchAddresses is true , we must matter about address match
    if(matchAddresses){
      if(firstAddressAccess == Read){
        while((cursor < max)&&(!segment->containsAddressRead(address))){
          ++address;
          ++cursor;
        }
      }else{
        while((cursor < max)&&(!segment->containsAddressWrite(address))){
          ++address;
          ++cursor;
        }
      }
    }else{
      if(firstAddressAccess == Read){
        address = segment->startAddressRead();
      }else{
        address = segment->startAddressWrite();
      }
    }
  }
}

void mdtDeviceIos::setDigitalOutputsEnabled(bool enabled)
{
  int i;

  for(i=0; i<pvDigitalOutputs.size(); ++i){
    Q_ASSERT(pvDigitalOutputs.at(i) != 0);
    pvDigitalOutputs.at(i)->setEnabled(enabled);
  }
}
