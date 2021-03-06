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
#include "mdtPortInfoCbHandler.h"
#include "mdtPortInfo.h"
#include "mdtDeviceInfo.h"
#include <QComboBox>
#include <QVariant>

//#include <QDebug>

mdtPortInfoCbHandler::mdtPortInfoCbHandler(QObject *parent)
 : QObject(parent)
{
  cbPorts = 0;
  cbDevices = 0;
}

mdtPortInfoCbHandler::~mdtPortInfoCbHandler()
{
}

void mdtPortInfoCbHandler::setPortsComboBox(QComboBox *comboBox)
{
  Q_ASSERT(comboBox != 0);

  cbPorts = comboBox;
}

void mdtPortInfoCbHandler::setDevicesComboBox(QComboBox *comboBox)
{
  Q_ASSERT(comboBox != 0);
  Q_ASSERT(cbPorts != 0);

  if(cbDevices != 0){
    disconnect(cbPorts, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDevicesCb(int)));
  }
  cbDevices = comboBox;
  connect(cbPorts, SIGNAL(currentIndexChanged(int)), this, SLOT(updateDevicesCb(int)));
}

void mdtPortInfoCbHandler::fillComboBoxes(const QList<mdtPortInfo*> &portInfoList)
{
  Q_ASSERT(cbPorts != 0);

  int i;
  mdtPortInfo *portInfo;

  cbPorts->clear();
  for(i=0; i<portInfoList.size(); i++){
    portInfo = portInfoList.at(i);
    Q_ASSERT(portInfo != 0);
    addPortInfo(*portInfo);
  }
}

void mdtPortInfoCbHandler::addPortInfo(const mdtPortInfo &portInfo)
{
  Q_ASSERT(cbPorts != 0);

  QVariant var;

  var.setValue(portInfo);
  cbPorts->addItem(portInfo.displayText(), var);
}

mdtPortInfo mdtPortInfoCbHandler::portInfoAt(int index)
{
  Q_ASSERT(cbPorts != 0);

  mdtPortInfo portInfo;

  // Check range
  if(index < 0){
    return portInfo;
  }
  if(index >= cbPorts->count()){
    return portInfo;
  }

  return cbPorts->itemData(index).value<mdtPortInfo>();
}

mdtPortInfo mdtPortInfoCbHandler::currentPortInfo()
{
  Q_ASSERT(cbPorts != 0);

  return portInfoAt(cbPorts->currentIndex());
}

int mdtPortInfoCbHandler::indexOfPortInfo(const mdtPortInfo &portInfo) const
{
  Q_ASSERT(cbPorts != 0);

  int i;

  for(i=0; i<cbPorts->count(); ++i){
    if(cbPorts->itemData(i).value<mdtPortInfo>() == portInfo){
      return i;
    }
  }

  return -1;
}

mdtDeviceInfo mdtPortInfoCbHandler::currentDeviceInfo()
{
  mdtDeviceInfo deviceInfo;

  // Devices comboBox is optional
  if(cbDevices == 0){
    return deviceInfo;
  }
  if(cbDevices->currentIndex() < 0){
    return deviceInfo;
  }

  return cbDevices->itemData(cbDevices->currentIndex()).value<mdtDeviceInfo>();
}

void mdtPortInfoCbHandler::updateDevicesCb(int portsCbIndex)
{
  Q_ASSERT(cbPorts != 0);

  int i;
  mdtPortInfo portInfo;
  mdtDeviceInfo *deviceInfo;
  QVariant var;

  // Devices comboBox is optional
  if(cbDevices == 0){
    return;
  }
  cbDevices->clear();
  // On invalid index we have nothing to fill devices comboBox
  if(portsCbIndex < 0){
    return;
  }
  // Get ports comboBox current item
  var = cbPorts->itemData(portsCbIndex);
  portInfo = var.value<mdtPortInfo>();
  // Fill devices comboBox
  for(i=0; i<portInfo.deviceInfoList().size(); i++){
    deviceInfo = portInfo.deviceInfoList().at(i);
    Q_ASSERT(deviceInfo != 0);
    var.setValue(*deviceInfo);
    cbDevices->addItem(deviceInfo->displayText(), var);
  }
}
