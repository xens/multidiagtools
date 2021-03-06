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
#include "mdtDeviceTest.h"
#include "mdtApplication.h"
#include "mdtDeviceContainer.h"
#include "mdtDeviceIos.h"
#include "mdtDeviceIosSegment.h"
#include "mdtDeviceIosWidget.h"
#include "mdtMultiIoDevice.h"
#include "mdtDeviceModbus.h"
#include "mdtModbusTcpPortManager.h"
#include "mdtDeviceModbusWago.h"
#include "mdtDeviceModbusWagoModule.h"
#include "mdtDeviceModbusWagoModuleRtd.h"
#include "mdtDeviceScpi.h"
#include "mdtDeviceU3606A.h"
#include "mdtFrameCodecScpiU3606A.h"
#include "mdtDeviceDSO1000A.h"
#include "mdtDeviceWindow.h"
#include "mdtPortInfo.h"
#include "mdtPortManager.h"
#include "mdtPortConfig.h"
#include "mdtUsbtmcPortSetupDialog.h"
#include "mdtUsbDeviceDescriptor.h"

///#include "mdtFrameCodecScpi.h"

#include <QString>
#include <QVariant>
#include <QList>

#include <QTime>
#include <QTimer>
#include <QDebug>

#include <memory>
#include <vector>
#include <typeinfo>
///#include <chrono>
#include <boost/any.hpp>

void mdtDeviceTest::sandbox()
{
  
  
  
  ///std::shared_ptr<mdtDevice> dev1(new mdtDevice);
  ///std::shared_ptr<mdtDevice> dev2(new mdtDeviceU3606A);

 /// Q_ASSERT(dev1);
  ///Q_ASSERT(dev2);

  ///std::shared_ptr<mdtDeviceU3606A> devU3606;

  ///Q_ASSERT(std::dynamic_pointer_cast<mdtDeviceU3606A>(dev2));
  ///devU3606 = std::static_pointer_cast<mdtDeviceU3606A>(dev2);
  ///Q_ASSERT(devU3606);
  ///devU3606->setName("U3606A");
  
//   qDebug() << "Name: " << devU3606->name() << " , " << dev2->name();
//   qDebug() << "Value: " << devU3606->getMeasureValue();
//   qDebug() << "dev1 type: " << typeid(dev1).name();
//   qDebug() << "dev2 type: " << typeid(dev2).name();
//   qDebug() << "devU3606 type: " << typeid(devU3606).name();
//   
//   boost::any adev1;
//   boost::any adev3;
// 
//   adev1 = dev1;
//   adev3 = devU3606;
//   
//   qDebug() << "dev1 type: " << typeid(dev1).name() << " , adev1 type: " << adev1.type().name();
//   qDebug() << "devU3606 type: " << typeid(devU3606).name() << " , adev3 type: " << adev3.type().name();
}

void mdtDeviceTest::deviceBaseTest()
{
  mdtDevice dev;
  mdtDeviceAddress address;

  // Initial states
  QVERIFY(!dev.isReady());
  QCOMPARE(dev.alias(), QString(""));
  ///QVERIFY(dev.state() == mdtDevice::Disconnected);
  ///QVERIFY(dev.state() == mdtPortManager::Disconnected);

  /*
   * Device address
   */
  address.setUsbIdentification(0x1234, 0x5678, "SN004", 0);
  dev.setDeviceAddress(address, "PowerSupply");
  QCOMPARE(dev.alias(), QString("PowerSupply"));
  QVERIFY(!dev.connectToDevice());

  dev.start(100);
}

void mdtDeviceTest::deviceContainerTest()
{
  mdtDeviceContainer c;
  mdtDeviceAddress deviceAddress;
  std::shared_ptr<mdtDeviceU3606A> dev;

  // Initial state
  QCOMPARE(c.deviceCount(), 0);
  QCOMPARE(c.allDevices().size(), 0);
  /*
   * Add a U3606A device
   * Version that take a mdtDeviceAddress as argument
   */
  deviceAddress.setUsbIdentification(0x0957, 0x4d18, "", 0);
  deviceAddress.setAlias("U3606A");
  dev = c.addDevice<mdtDeviceU3606A>(deviceAddress);
  QVERIFY(dev.get() != 0);
  QCOMPARE((int)dev->deviceAddress().usbIdVendor(), 0x0957);
  QCOMPARE((int)dev->deviceAddress().usbIdProduct(), 0x4d18);
  // Check that we can find added device
  QVERIFY(c.device<mdtDeviceU3606A>("U3606A").get() != 0);
  QVERIFY(c.device<mdtDeviceU3606A>("").get() == 0);
  QVERIFY(c.device<mdtDevice>("U3606A").get() != 0);
  QVERIFY(c.device<mdtDeviceScpi>("U3606A").get() != 0);
  QVERIFY(c.device<mdtDeviceModbus>("U3606A").get() == 0);
  QCOMPARE(c.deviceCount(), 1);
  QCOMPARE(c.allDevices().size(), 1);
  QVERIFY(c.allDevices().at(0).get() != 0);
  // Clear
  c.clear();
  QCOMPARE(c.deviceCount(), 0);
  QCOMPARE(c.allDevices().size(), 0);
  QVERIFY(c.device<mdtDeviceU3606A>("U3606A").get() == 0);
  /*
   * Add a U3606A device
   * Version that take a alias
   */
  dev = c.addDevice<mdtDeviceU3606A>("U3606A");
  QVERIFY(dev.get() != 0);
  QCOMPARE((int)dev->deviceAddress().usbIdVendor(), 0);
  QCOMPARE((int)dev->deviceAddress().usbIdProduct(), 0);
  // Connect and check that device address was updated
  if(!dev->connectToDevice("U3606A")){
    QSKIP("No Agilent U3606A device attached, cannot finish test.", SkipAll);
  }
  QCOMPARE((int)dev->deviceAddress().usbIdVendor(), 0x0957);
  QCOMPARE((int)dev->deviceAddress().usbIdProduct(), 0x4d18);
  QVERIFY(c.device<mdtDeviceU3606A>("U3606A").get() != 0);

}

void mdtDeviceTest::globalDeviceContainerTest()
{
  mdtGlobalDeviceContainer gc1, gc2;
  mdtGlobalDeviceContainer *gc3, *gc4;
  std::shared_ptr<mdtDeviceContainer> c;
  std::shared_ptr<mdtDevice> dev;

  /*
   * Add a device with one global container
   *  and check that other contains the same
   */
  // Add device and check
  dev = gc1->addDevice<mdtDeviceU3606A>("DMM1");
  QVERIFY(dev.get() != 0);
  QCOMPARE((int)dev->deviceAddress().usbIdVendor(), 0);
  QCOMPARE((int)dev->deviceAddress().usbIdProduct(), 0);
  // Check accessing global container from gc1
  QCOMPARE(gc1->deviceCount(), 1);
  dev = gc1->device<mdtDeviceU3606A>("DMM1");
  QVERIFY(dev.get() != 0);
  QCOMPARE(dev->alias(), QString("DMM1"));
  // Check accessing global container from gc2
  QCOMPARE(gc2->deviceCount(), 1);
  dev = gc2->device<mdtDeviceU3606A>("DMM1");
  QVERIFY(dev.get() != 0);
  QCOMPARE(dev->alias(), QString("DMM1"));
  /*
   * Check global container object creation and destruction
   */
  // Create gc3 instance
  gc3 = new mdtGlobalDeviceContainer;
  // Check accessing global container from gc3
  QCOMPARE((*gc3)->deviceCount(), 1);
  dev = (*gc3)->device<mdtDeviceU3606A>("DMM1");
  QVERIFY(dev.get() != 0);
  QCOMPARE(dev->alias(), QString("DMM1"));
  // Create gc4 instance
  gc4 = new mdtGlobalDeviceContainer;
  // Check accessing global container from gc3
  QCOMPARE((*gc4)->deviceCount(), 1);
  dev = (*gc4)->device<mdtDeviceU3606A>("DMM1");
  QVERIFY(dev.get() != 0);
  QCOMPARE(dev->alias(), QString("DMM1"));
  // Remove gc3 instance
  delete gc3;
  // Check gc1 instance
  QCOMPARE(gc1->deviceCount(), 1);
  dev = gc1->device<mdtDeviceU3606A>("DMM1");
  QVERIFY(dev.get() != 0);
  QCOMPARE(dev->alias(), QString("DMM1"));
  // Check gc4 instance
  QCOMPARE((*gc4)->deviceCount(), 1);
  dev = (*gc4)->device<mdtDeviceU3606A>("DMM1");
  QVERIFY(dev.get() != 0);
  QCOMPARE(dev->alias(), QString("DMM1"));
  // Remove gc4 instance
  delete gc4;
  // Check gc1 instance
  QCOMPARE(gc1->deviceCount(), 1);
  dev = gc1->device<mdtDeviceU3606A>("DMM1");
  QVERIFY(dev.get() != 0);
  QCOMPARE(dev->alias(), QString("DMM1"));
}

void mdtDeviceTest::deviceIosSegmentTest()
{
  mdtDeviceIosSegment seg;
  mdtAnalogIo *aIo;
  mdtDigitalIo *dIo;
  QList<mdtAnalogIo*> aIoList;
  QList<mdtDigitalIo*> dIoList;
  QList<int> expectedReadAddresses;
  QList<int> expectedWriteAddresses;
  QList<int> expectedValuesInt;
  QList<double> expectedValuesDouble;
  QList<bool> expectedValuesBool;
  QList<mdtValue> valuesMdt;
  QList<QVariant> valuesVar;

  // Check default values
  QCOMPARE(seg.startAddressRead(), 0);
  QCOMPARE(seg.startAddressWrite(), 0);
  QCOMPARE(seg.endAddressRead(), 0);
  QCOMPARE(seg.endAddressWrite(), 0);
  QCOMPARE(seg.ioCount(), 0);

  /*
   * Analog I/O
   */

  // Build a list of 9 I/O's
  expectedReadAddresses.clear();
  expectedWriteAddresses.clear();
  expectedValuesInt.clear();
  expectedValuesDouble.clear();
  for(int i = 0; i < 9; i++){
    aIo = new mdtAnalogIo;
    aIo->setAddressRead(i);
    expectedReadAddresses.append(i);
    aIo->setAddressWrite(i+100);
    expectedWriteAddresses.append(i+100);
    aIo->setValue(3*i);
    expectedValuesInt.append(3*i);
    expectedValuesDouble.append(3*i);
    aIoList.append(aIo);
  }
  // Set I/O's and check ...
  seg.setIos(aIoList);
  QCOMPARE(seg.startAddressRead(), 0);
  QCOMPARE(seg.startAddressWrite(), 100);
  QCOMPARE(seg.endAddressRead(), 8);
  QCOMPARE(seg.endAddressWrite(), 108);
  QVERIFY(!seg.containsAddressRead(-1));
  QVERIFY(seg.containsAddressRead(0));
  QVERIFY(seg.containsAddressRead(1));
  QVERIFY(seg.containsAddressRead(2));
  QVERIFY(seg.containsAddressRead(3));
  QVERIFY(seg.containsAddressRead(4));
  QVERIFY(seg.containsAddressRead(5));
  QVERIFY(seg.containsAddressRead(6));
  QVERIFY(seg.containsAddressRead(7));
  QVERIFY(seg.containsAddressRead(8));
  QVERIFY(!seg.containsAddressRead(9));
  QVERIFY(!seg.containsAddressWrite(99));
  QVERIFY(seg.containsAddressWrite(100));
  QVERIFY(seg.containsAddressWrite(101));
  QVERIFY(seg.containsAddressWrite(102));
  QVERIFY(seg.containsAddressWrite(103));
  QVERIFY(seg.containsAddressWrite(104));
  QVERIFY(seg.containsAddressWrite(105));
  QVERIFY(seg.containsAddressWrite(106));
  QVERIFY(seg.containsAddressWrite(107));
  QVERIFY(seg.containsAddressWrite(108));
  QVERIFY(!seg.containsAddressWrite(109));
  QCOMPARE(seg.ioCount(), 9);
  QCOMPARE(seg.addressesRead(), expectedReadAddresses);
  QCOMPARE(seg.addressesWrite(), expectedWriteAddresses);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  QCOMPARE(seg.valuesDouble(), expectedValuesDouble);
  // Set some values and check - correct amount of values - type: mdtValue
  valuesMdt.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 9; i++){
    valuesMdt.append(2*i);
    expectedValuesInt.append(2*i);
  }
  QCOMPARE(seg.setValues(valuesMdt), 9);
  QCOMPARE(seg.values().size(), 9);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  // Set some values and check - correct amount of values - type: QVariant
  valuesVar.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 9; i++){
    valuesVar.append(5*i);
    expectedValuesInt.append(5*i);
  }
  QCOMPARE(seg.setValues(valuesVar), 9);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  // Set some values and check - to less values - type: mdtValue
  valuesMdt.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 5; i++){
    valuesMdt.append(7*i);
    expectedValuesInt.append(7*i);
  }
  QCOMPARE(seg.setValues(valuesMdt), 5);
  QCOMPARE(seg.values().size(), 9);
  for(int i = 0; i < 5; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueInt(), expectedValuesInt.at(i));
    QCOMPARE(seg.valuesInt().at(i), expectedValuesInt.at(i));
  }
  QCOMPARE(seg.values().at(5).valueInt(), 5*5);
  QCOMPARE(seg.values().at(6).valueInt(), 5*6);
  QCOMPARE(seg.values().at(7).valueInt(), 5*7);
  QCOMPARE(seg.values().at(8).valueInt(), 5*8);
  // Set some values and check - to less values - type: QVariant
  valuesVar.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 3; i++){
    valuesVar.append(3*i);
    expectedValuesInt.append(3*i);
  }
  QCOMPARE(seg.setValues(valuesVar), 3);
  QCOMPARE(seg.values().size(), 9);
  for(int i = 0; i < 3; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueInt(), expectedValuesInt.at(i));
    QCOMPARE(seg.valuesInt().at(i), expectedValuesInt.at(i));
  }
  QCOMPARE(seg.values().at(3).valueInt(), 7*3);
  QCOMPARE(seg.values().at(4).valueInt(), 7*4);
  QCOMPARE(seg.values().at(5).valueInt(), 5*5);
  QCOMPARE(seg.values().at(6).valueInt(), 5*6);
  QCOMPARE(seg.values().at(7).valueInt(), 5*7);
  QCOMPARE(seg.values().at(8).valueInt(), 5*8);
  // Set some values and check - to many values - type: mdtValue
  valuesMdt.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 20; i++){
    valuesMdt.append(3*i);
    expectedValuesInt.append(3*i);
  }
  QCOMPARE(seg.setValues(valuesMdt), 9);
  QCOMPARE(seg.values().size(), 9);
  for(int i = 0; i < 9; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueInt(), expectedValuesInt.at(i));
    QCOMPARE(seg.valuesInt().at(i), expectedValuesInt.at(i));
  }
  // Set some values and check - to many values - type: QVariant
  valuesVar.clear();
  expectedValuesInt.clear();
  for(int i = 0; i < 30; i++){
    valuesVar.append(2*i);
    expectedValuesInt.append(2*i);
  }
  QCOMPARE(seg.setValues(valuesVar), 9);
  QCOMPARE(seg.values().size(), 9);
  for(int i = 0; i < 9; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueInt(), expectedValuesInt.at(i));
    QCOMPARE(seg.valuesInt().at(i), expectedValuesInt.at(i));
  }
  // Set some values and check - to many values with max specified - type: mdtValue
  valuesMdt.clear();
  valuesMdt << 0 << 0 << 0 << 0 << 0;
  seg.setValues(valuesMdt);
  QVERIFY(seg.values().size() >  4);
  QCOMPARE(seg.values().at(0).valueInt(), 0);
  QCOMPARE(seg.values().at(1).valueInt(), 0);
  QCOMPARE(seg.values().at(2).valueInt(), 0);
  QCOMPARE(seg.values().at(3).valueInt(), 0);
  QCOMPARE(seg.values().at(4).valueInt(), 0);
  valuesMdt.clear();
  valuesMdt << 0 << -10 << -20 << -30 << -40;
  seg.setValues(valuesMdt, 3);
  QVERIFY(seg.values().size() >  4);
  QCOMPARE(seg.values().at(0).valueInt(), 0);
  QCOMPARE(seg.values().at(1).valueInt(), -10);
  QCOMPARE(seg.values().at(2).valueInt(), -20);
  QCOMPARE(seg.values().at(3).valueInt(), 0);
  QCOMPARE(seg.values().at(4).valueInt(), 0);
  // Set some values and check - to many values with max specified - type: QVariant
  valuesVar.clear();
  valuesVar << 0 << 0 << 0 << 0 << 0;
  seg.setValues(valuesVar);
  QVERIFY(seg.values().size() >  4);
  QCOMPARE(seg.values().at(0).valueInt(), 0);
  QCOMPARE(seg.values().at(1).valueInt(), 0);
  QCOMPARE(seg.values().at(2).valueInt(), 0);
  QCOMPARE(seg.values().at(3).valueInt(), 0);
  QCOMPARE(seg.values().at(4).valueInt(), 0);
  valuesVar.clear();
  valuesVar << 0 << -11 << -22 << -32 << -42;
  seg.setValues(valuesVar, 3);
  QVERIFY(seg.values().size() >  4);
  QCOMPARE(seg.values().at(0).valueInt(), 0);
  QCOMPARE(seg.values().at(1).valueInt(), -11);
  QCOMPARE(seg.values().at(2).valueInt(), -22);
  QCOMPARE(seg.values().at(3).valueInt(), 0);
  QCOMPARE(seg.values().at(4).valueInt(), 0);
  // Change a value of last aIo and check that it is reflected
  QVERIFY(seg.valuesInt().at(8) != 11);
  aIo->setValue(11);
  QCOMPARE(seg.valuesInt().at(8), 11);
  // Update some values and check - Address read - type: mdtValue
  expectedValuesInt = seg.valuesInt();
  expectedValuesInt[0] = -10;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -10;
  QCOMPARE(seg.updateValuesFromAddressRead(0, valuesMdt), 1);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[2] = -20;
  expectedValuesInt[3] = -30;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -20 << -30;
  QCOMPARE(seg.updateValuesFromAddressRead(2, valuesMdt), 2);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[6] = -60;
  expectedValuesInt[7] = -70;
  expectedValuesInt[8] = -80;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -60 << -70 << -80 << -90 << -100;
  QCOMPARE(seg.updateValuesFromAddressRead(6, valuesMdt), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  valuesMdt << -2000 << -3000;
  QCOMPARE(seg.updateValuesFromAddressRead(40, valuesMdt), 0);
  // Update values for I/O's having address read 2-4 (3 items) with a list of 4 items (max test) - Type: mdtValue
  QVERIFY(expectedValuesInt[1] != 20);
  QVERIFY(expectedValuesInt[5] != 50);
  expectedValuesInt[2] = 20;
  expectedValuesInt[3] = 30;
  expectedValuesInt[4] = 40;
  valuesMdt.clear();
  valuesMdt << 20 << 30 << 40 << 50;
  QCOMPARE(seg.updateValuesFromAddressRead(2, valuesMdt, 3), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  // Update some values and check - Address read - type: QVariant
  expectedValuesInt = seg.valuesInt();
  expectedValuesInt[0] = -100;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -100;
  QCOMPARE(seg.updateValuesFromAddressRead(0, valuesVar), 1);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[2] = -200;
  expectedValuesInt[3] = -300;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -200 << -300;
  QCOMPARE(seg.updateValuesFromAddressRead(2, valuesVar), 2);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[6] = -600;
  expectedValuesInt[7] = -700;
  expectedValuesInt[8] = -800;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -600 << -700 << -800 << -900 << -1000;
  QCOMPARE(seg.updateValuesFromAddressRead(6, valuesVar), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  valuesVar << -2000 << -3000;
  QCOMPARE(seg.updateValuesFromAddressRead(40, valuesVar), 0);
  // Update values for I/O's having address read 2-4 (3 items) with a list of 4 items (max test) - Type: QVariant
  QVERIFY(expectedValuesInt[1] != 21);
  QVERIFY(expectedValuesInt[5] != 51);
  expectedValuesInt[2] = 21;
  expectedValuesInt[3] = 31;
  expectedValuesInt[4] = 41;
  valuesVar.clear();
  valuesVar << 21 << 31 << 41 << 51;
  QCOMPARE(seg.updateValuesFromAddressRead(2, valuesVar, 3), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  // Update some values and check - Address write - type: mdtValue
  expectedValuesInt = seg.valuesInt();
  expectedValuesInt[0] = -10;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -10;
  QCOMPARE(seg.updateValuesFromAddressWrite(100, valuesMdt), 1);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[2] = -20;
  expectedValuesInt[3] = -30;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -20 << -30;
  QCOMPARE(seg.updateValuesFromAddressWrite(102, valuesMdt), 2);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[6] = -60;
  expectedValuesInt[7] = -70;
  expectedValuesInt[8] = -80;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesMdt.clear();
  valuesMdt << -60 << -70 << -80 << -90 << -100;
  QCOMPARE(seg.updateValuesFromAddressWrite(106, valuesMdt), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  valuesMdt << -2000 << -3000;
  QCOMPARE(seg.updateValuesFromAddressWrite(140, valuesMdt), 0);
  // Update values for I/O's having address write 102-104 (3 items) with a list of 4 items (max test) - Type: mdtValue
  QVERIFY(expectedValuesInt[1] != 20);
  QVERIFY(expectedValuesInt[5] != 50);
  expectedValuesInt[2] = 20;
  expectedValuesInt[3] = 30;
  expectedValuesInt[4] = 40;
  valuesMdt.clear();
  valuesMdt << 20 << 30 << 40 << 50;
  QCOMPARE(seg.updateValuesFromAddressWrite(102, valuesMdt, 3), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  // Update some values and check - Address write - type: QVariant
  expectedValuesInt = seg.valuesInt();
  expectedValuesInt[0] = -100;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -100;
  QCOMPARE(seg.updateValuesFromAddressWrite(100, valuesVar), 1);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[2] = -200;
  expectedValuesInt[3] = -300;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -200 << -300;
  QCOMPARE(seg.updateValuesFromAddressWrite(102, valuesVar), 2);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  expectedValuesInt[6] = -600;
  expectedValuesInt[7] = -700;
  expectedValuesInt[8] = -800;
  QVERIFY(!(seg.valuesInt() == expectedValuesInt));
  valuesVar.clear();
  valuesVar << -600 << -700 << -800 << -900 << -1000;
  QCOMPARE(seg.updateValuesFromAddressWrite(106, valuesVar), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);
  valuesVar << -2000 << -3000;
  QCOMPARE(seg.updateValuesFromAddressWrite(140, valuesVar), 0);
  // Update values for I/O's having address write 102-104 (3 items) with a list of 4 items (max test) - Type: QVariant
  QVERIFY(expectedValuesInt[1] != 21);
  QVERIFY(expectedValuesInt[5] != 51);
  expectedValuesInt[2] = 21;
  expectedValuesInt[3] = 31;
  expectedValuesInt[4] = 41;
  valuesVar.clear();
  valuesVar << 21 << 31 << 41 << 51;
  QCOMPARE(seg.updateValuesFromAddressWrite(102, valuesVar, 3), 3);
  QCOMPARE(seg.valuesInt(), expectedValuesInt);

  /*
   * Digital I/O
   */

  // Build a list of 10 I/O's
  expectedReadAddresses.clear();
  expectedWriteAddresses.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 10; i++){
    dIo = new mdtDigitalIo;
    dIo->setAddressRead(i);
    expectedReadAddresses.append(i);
    dIo->setAddressWrite(i+100);
    expectedWriteAddresses.append(i+100);
    dIo->setValue((i%2)==0);
    expectedValuesBool.append((i%2)==0);
    dIoList.append(dIo);
  }
  // Set I/O's and check ...
  seg.setIos(dIoList);
  QCOMPARE(seg.startAddressRead(), 0);
  QCOMPARE(seg.startAddressWrite(), 100);
  QCOMPARE(seg.endAddressRead(), 9);
  QCOMPARE(seg.endAddressWrite(), 109);
  QVERIFY(!seg.containsAddressRead(-1));
  QVERIFY(seg.containsAddressRead(0));
  QVERIFY(seg.containsAddressRead(1));
  QVERIFY(seg.containsAddressRead(2));
  QVERIFY(seg.containsAddressRead(3));
  QVERIFY(seg.containsAddressRead(4));
  QVERIFY(seg.containsAddressRead(5));
  QVERIFY(seg.containsAddressRead(6));
  QVERIFY(seg.containsAddressRead(7));
  QVERIFY(seg.containsAddressRead(8));
  QVERIFY(seg.containsAddressRead(9));
  QVERIFY(!seg.containsAddressRead(10));
  QVERIFY(!seg.containsAddressWrite(99));
  QVERIFY(seg.containsAddressWrite(100));
  QVERIFY(seg.containsAddressWrite(101));
  QVERIFY(seg.containsAddressWrite(102));
  QVERIFY(seg.containsAddressWrite(103));
  QVERIFY(seg.containsAddressWrite(104));
  QVERIFY(seg.containsAddressWrite(105));
  QVERIFY(seg.containsAddressWrite(106));
  QVERIFY(seg.containsAddressWrite(107));
  QVERIFY(seg.containsAddressWrite(108));
  QVERIFY(seg.containsAddressWrite(109));
  QVERIFY(!seg.containsAddressWrite(110));
  QCOMPARE(seg.ioCount(), 10);
  QCOMPARE(seg.addressesRead(), expectedReadAddresses);
  QCOMPARE(seg.addressesWrite(), expectedWriteAddresses);
  QCOMPARE(seg.valuesBool(), expectedValuesBool);
  // Set some values and check - correct amount of values - type: mdtValue
  valuesMdt.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 10; i++){
    valuesMdt.append((i%2)!=0);
    expectedValuesBool.append((i%2)!=0);
  }
  QCOMPARE(seg.setValues(valuesMdt), 10);
  QCOMPARE(seg.values().size(), 10);
  QCOMPARE(seg.valuesBool(), expectedValuesBool);
  // Set some values and check - correct amount of values - type: QVariant
  valuesVar.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 10; i++){
    valuesVar.append((i%2)==0);
    expectedValuesBool.append((i%2)==0);
  }
  QCOMPARE(seg.setValues(valuesVar), 10);
  QCOMPARE(seg.valuesBool(), expectedValuesBool);
  // Set some values and check - to less values - type: mdtValue
  valuesMdt.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 5; i++){
    valuesMdt.append((i%2)!=0);
    expectedValuesBool.append((i%2)!=0);
  }
  QCOMPARE(seg.setValues(valuesMdt), 5);
  QCOMPARE(seg.values().size(), 10);
  for(int i = 0; i < 5; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueBool(), expectedValuesBool.at(i));
    QCOMPARE(seg.valuesBool().at(i), expectedValuesBool.at(i));
  }
  QCOMPARE(seg.values().at(6).valueBool(), (6%2)==0);
  QCOMPARE(seg.values().at(7).valueBool(), (7%2)==0);
  QCOMPARE(seg.values().at(8).valueBool(), (8%2)==0);
  // Set some values and check - to less values - type: QVariant
  valuesVar.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 3; i++){
    valuesVar.append((i%2)==0);
    expectedValuesBool.append((i%2)==0);
  }
  QCOMPARE(seg.setValues(valuesVar), 3);
  QCOMPARE(seg.values().size(), 10);
  for(int i = 0; i < 3; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueBool(), expectedValuesBool.at(i));
    QCOMPARE(seg.valuesBool().at(i), expectedValuesBool.at(i));
  }
  QCOMPARE(seg.values().at(3).valueBool(), (3%2)!= 0);
  QCOMPARE(seg.values().at(4).valueBool(), (4%2)!= 0);
  QCOMPARE(seg.values().at(5).valueBool(), (5%2)==0);
  QCOMPARE(seg.values().at(6).valueBool(), (6%2)==0);
  QCOMPARE(seg.values().at(7).valueBool(), (7%2)==0);
  QCOMPARE(seg.values().at(8).valueBool(), (8%2)==0);
  // Set some values and check - to many values - type: mdtValue
  valuesMdt.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 20; i++){
    valuesMdt.append((i%2)!=0);
    expectedValuesBool.append((i%2)!=0);
  }
  QCOMPARE(seg.setValues(valuesMdt), 10);
  QCOMPARE(seg.values().size(), 10);
  for(int i = 0; i < 9; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueBool(), expectedValuesBool.at(i));
    QCOMPARE(seg.valuesBool().at(i), expectedValuesBool.at(i));
  }
  // Set some values and check - to many values - type: QVariant
  valuesVar.clear();
  expectedValuesBool.clear();
  for(int i = 0; i < 30; i++){
    valuesVar.append((i%2)==0);
    expectedValuesBool.append((i%2)==0);
  }
  QCOMPARE(seg.setValues(valuesVar), 10);
  QCOMPARE(seg.values().size(), 10);
  for(int i = 0; i < 9; i++){
    QVERIFY(seg.values().at(i).isValid());
    QCOMPARE(seg.values().at(i).valueBool(), expectedValuesBool.at(i));
    QCOMPARE(seg.valuesBool().at(i), expectedValuesBool.at(i));
  }
  // Change value of last aIo and check that it is reflected
  dIo->setValue(true);
  QCOMPARE(seg.valuesBool().at(9), true);
  dIo->setValue(false);
  QCOMPARE(seg.valuesBool().at(9), false);
  // Update some values and check - type: mdtValue
  
  // Update some values and check - type: QVariant

}

void mdtDeviceTest::deviceIosTest()
{
  mdtDeviceIos ios;
  mdtAnalogIo *ai;
  QList<mdtAnalogIo*> aIoList;
  mdtAnalogIo *ao;
  mdtDigitalIo *di;
  QList<mdtDigitalIo*> dIoList;
  mdtDigitalIo *dout;
  QList<QVariant> values;
  QList<bool> doSates;

  // Add analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(12);
  ai->setRange(-100.0, 100.0, 24, 0, true);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(10);
  ai->setRange(-100.0, 100.0, 24, 0, true);
  ai->setLabelShort("AI10");
  ios.addAnalogInput(ai);

  // Add analog outputs
  ao = new mdtAnalogIo;
  ao->setAddressRead(15);
  ao->setAddressWrite(115);
  ao->setRange(-100.0, 100.0, 24, 0, true);
  ios.addAnalogOutput(ao);
  ao = new mdtAnalogIo;
  ao->setAddressRead(8);
  ao->setAddressWrite(108);
  ao->setLabelShort("AO8");
  ao->setRange(-100.0, 100.0, 24, 0, true);
  ios.addAnalogOutput(ao);

  // Add digital inputs
  di = new mdtDigitalIo;
  di->setAddress(17);
  ios.addDigitalInput(di);
  di = new mdtDigitalIo;
  di->setAddress(18);
  di->setLabelShort("DI18");
  ios.addDigitalInput(di);

  // Add digital outputs
  dout = new mdtDigitalIo;
  dout->setAddressRead(20);
  dout->setAddressWrite(120);
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddressRead(30);
  dout->setAddressWrite(130);
  dout->setLabelShort("DO30");
  ios.addDigitalOutput(dout);

  // Check analog inputs
  QCOMPARE(ios.analogInputsCount(), 2);
  QVERIFY(ios.analogInputAtAddress(10) != 0);
  QCOMPARE(ios.analogInputAtAddress(10)->address() , 10);
  QVERIFY(ios.analogInputWithLabelShort("AI10") != 0);
  QCOMPARE(ios.analogInputWithLabelShort("AI10")->address(), 10);
  QCOMPARE(ios.analogInputWithLabelShort("AI10")->labelShort(), QString("AI10"));
  QVERIFY(ios.analogInputAtAddress(11) == 0);
  QVERIFY(ios.analogInputAtAddress(12) != 0);
  QCOMPARE(ios.analogInputAtAddress(12)->address() , 12);
  ios.analogInputAtAddress(12)->setLabelShort("Temp. M12");
  QVERIFY(ios.analogInputWithLabelShort("") == 0);
  QVERIFY(ios.analogInputWithLabelShort("Temp. M12") != 0);
  QCOMPARE(ios.analogInputWithLabelShort("Temp. M12")->address(), 12);
  QCOMPARE(ios.analogInputWithLabelShort("Temp. M12")->labelShort(), QString("Temp. M12"));
  aIoList = ios.analogInputs();
  QCOMPARE(aIoList.size(), 2);
  // Check multiple analog inputs update
  values.clear();
  values << 1.0 << 12.5;
  ios.updateAnalogInputValues(values, -1, -1, false);
  QCOMPARE(ios.analogInputAtAddress(10)->value().valueDouble() , 1.0);
  QCOMPARE(ios.analogInputAtAddress(12)->value().valueDouble() , 12.5);
  values.clear();
  values << 2.0 << 3.0;
  ios.updateAnalogInputValues(values, -1, 1, false);
  QCOMPARE(ios.analogInputAtAddress(10)->value().valueDouble() , 2.0);
  QCOMPARE(ios.analogInputAtAddress(12)->value().valueDouble() , 12.5);
  values.clear();
  values << 4.0 << 5.0;
  ios.updateAnalogInputValues(values, 10, -1, false);
  QCOMPARE(ios.analogInputAtAddress(10)->value().valueDouble() , 4.0);
  QCOMPARE(ios.analogInputAtAddress(12)->value().valueDouble() , 5.0);
  values.clear();
  values << 6.0 << 7.0;
  ios.updateAnalogInputValues(values, 10, 1, false);
  QCOMPARE(ios.analogInputAtAddress(10)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogInputAtAddress(12)->value().valueDouble() , 5.0);
  values.clear();
  values << 8.0;
  ios.updateAnalogInputValues(values, 12, 1, false);
  QCOMPARE(ios.analogInputAtAddress(10)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogInputAtAddress(12)->value().valueDouble() , 8.0);
  values.clear();
  values << 9.0 << 10.0;
  ios.updateAnalogInputValues(values, 12, 1, false);
  QCOMPARE(ios.analogInputAtAddress(10)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogInputAtAddress(12)->value().valueDouble() , 9.0);
  // We add a analog input with address 13, so we have 10,12,13
  ai = new mdtAnalogIo;
  ai->setAddress(13);
  ai->setRange(-100.0, 100.0, 24, 0, true);
  ai->setValue(50.0);   // Set directly a known value
  ios.addAnalogInput(ai);
  QCOMPARE(ios.analogInputsCount(), 3);
  QVERIFY(ios.analogInputAtAddress(10) != 0);
  QVERIFY(ios.analogInputAtAddress(12) != 0);
  QVERIFY(ios.analogInputAtAddress(13) != 0);
  // Check multiple analog inputs update: 1 item starting at address 12 with 2 items in value
  values.clear();
  values << 1.0 << 8.0;
  ios.updateAnalogInputValues(values, 12, 1, false);
  QCOMPARE(ios.analogInputAtAddress(10)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogInputAtAddress(12)->value().valueDouble() , 1.0);
  QCOMPARE(ios.analogInputAtAddress(13)->value().valueDouble() , 50.0);

  // Check analog outputs
  QVERIFY(ios.analogOutputAtAddressRead(13) == 0);
  QVERIFY(ios.analogOutputAtAddressWrite(13) == 0);
  QVERIFY(ios.analogOutputAtAddressRead(15) != 0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->addressRead() , 15);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->addressWrite() , 115);
  QVERIFY(ios.analogOutputAtAddressWrite(15) == 0);
  QVERIFY(ios.analogOutputAtAddressWrite(115) != 0);
  QCOMPARE(ios.analogOutputAtAddressWrite(115)->addressRead() , 15);
  QCOMPARE(ios.analogOutputAtAddressWrite(115)->addressWrite() , 115);
  ios.analogOutputAtAddressRead(15)->setLabelShort("Setpoint 15");
  QVERIFY(ios.analogOutputWithLabelShort("") == 0);
  QVERIFY(ios.analogOutputWithLabelShort("Setpoint 15") != 0);
  QCOMPARE(ios.analogOutputWithLabelShort("Setpoint 15")->addressRead(), 15);
  QCOMPARE(ios.analogOutputWithLabelShort("Setpoint 15")->addressWrite(), 115);
  QCOMPARE(ios.analogOutputWithLabelShort("Setpoint 15")->labelShort(), QString("Setpoint 15"));
  QVERIFY(ios.analogOutputAtAddressRead(8) != 0);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->addressRead() , 8);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->addressWrite() , 108);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->labelShort() , QString("AO8"));
  QVERIFY(ios.analogOutputAtAddressWrite(108) != 0);
  QCOMPARE(ios.analogOutputAtAddressWrite(108)->addressRead() , 8);
  QCOMPARE(ios.analogOutputAtAddressWrite(108)->addressWrite() , 108);
  QCOMPARE(ios.analogOutputAtAddressWrite(108)->labelShort() , QString("AO8"));
  aIoList = ios.analogOutputs();
  QCOMPARE(aIoList.size() , 2);
  values.clear();
  values << 1.0 << 12.5;
  ios.updateAnalogOutputValues(values, -1, mdtDeviceIos::Read, -1, false);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 1.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 12.5);
  values.clear();
  values << 2.0 << 3.0;
  ios.updateAnalogOutputValues(values, -1, mdtDeviceIos::Read, 1, false);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 2.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 12.5);
  values.clear();
  values << 4.0 << 5.0;
  ios.updateAnalogOutputValues(values, 8, mdtDeviceIos::Read, -1, false);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 4.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 5.0);
  values.clear();
  values << 6.0 << 7.0;
  ios.updateAnalogOutputValues(values, 8, mdtDeviceIos::Read, 1, false);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 5.0);
  values.clear();
  values << 8.0;
  ios.updateAnalogOutputValues(values, 15, mdtDeviceIos::Read, 1, false);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 8.0);
  values.clear();
  values << 9.0 << 10.0;
  ios.updateAnalogOutputValues(values, 15, mdtDeviceIos::Read, 1, false);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 6.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 9.0);
  // We add a new analog output with address read 9 / write 109 (we have also 8/108, 9/109, 15/115)
  ao = new mdtAnalogIo;
  ao->setAddressRead(9);
  ao->setAddressWrite(109);
  ao->setRange(-100.0, 100.0, 24, 0, true);
  ao->setValue(60.0);
  ios.addAnalogOutput(ao);
  QCOMPARE(ios.analogOutputsCount(), 3);
  QVERIFY(ios.analogOutputAtAddressRead(8) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(9) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(15) != 0);
  QVERIFY(ios.analogOutputAtAddressWrite(108) != 0);
  QVERIFY(ios.analogOutputAtAddressWrite(109) != 0);
  QVERIFY(ios.analogOutputAtAddressWrite(115) != 0);
  QCOMPARE(ios.analogOutputAtAddressWrite(109)->value().valueDouble(), 60.0);
  // Check multiple analog outputs update starting at address read 8, 1 item, with a list of 2 items
  values.clear();
  values << 1.0 << 2.0;
  ios.updateAnalogOutputValues(values, 8, mdtDeviceIos::Read, 1, false);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 1.0);
  QCOMPARE(ios.analogOutputAtAddressRead(9)->value().valueDouble() , 60.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 9.0);
  // Check multiple analog outputs update starting at address write 108, 1 item, with a list of 2 items
  values.clear();
  values << 10.0 << 20.0;
  ios.updateAnalogOutputValues(values, 108, mdtDeviceIos::Write, 1, false);
  QCOMPARE(ios.analogOutputAtAddressRead(8)->value().valueDouble() , 10.0);
  QCOMPARE(ios.analogOutputAtAddressRead(9)->value().valueDouble() , 60.0);
  QCOMPARE(ios.analogOutputAtAddressRead(15)->value().valueDouble() , 9.0);

  // Check digital inputs
  QVERIFY(ios.digitalInputAtAddress(11) == 0);
  QVERIFY(ios.digitalInputAtAddress(17) != 0);
  QCOMPARE(ios.digitalInputAtAddress(17)->address(), 17);
  ios.digitalInputAtAddress(17)->setLabelShort("DI17");
  QVERIFY(ios.digitalInputWithLabelShort("") == 0);
  QVERIFY(ios.digitalInputWithLabelShort("DI17") != 0);
  QCOMPARE(ios.digitalInputWithLabelShort("DI17")->address(), 17);
  QCOMPARE(ios.digitalInputWithLabelShort("DI17")->labelShort(), QString("DI17"));
  QVERIFY(ios.digitalInputAtAddress(18) != 0);
  QCOMPARE(ios.digitalInputAtAddress(18)->address(), 18);
  QVERIFY(ios.digitalInputWithLabelShort("DI18") != 0);
  QCOMPARE(ios.digitalInputWithLabelShort("DI18")->address(), 18);
  QCOMPARE(ios.digitalInputWithLabelShort("DI18")->labelShort(), QString("DI18"));
  dIoList = ios.digitalInputs();
  QCOMPARE(dIoList.size(), 2);
  // Check multiple digital inputs update - List contains the correct number of items
  values.clear();
  values << false << true;
  ios.updateDigitalInputValues(values, -1, -1, false);
  QCOMPARE(ios.digitalInputAtAddress(17)->value().valueBool(), false);
  QCOMPARE(ios.digitalInputAtAddress(18)->value().valueBool(), true);
  values.clear();
  values << true;
  ios.updateDigitalInputValues(values, -1, 1, false);
  QCOMPARE(ios.digitalInputAtAddress(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAtAddress(18)->value().valueBool(), true);
  values.clear();
  values << false;
  ios.updateDigitalInputValues(values, 18, 1, false);
  QCOMPARE(ios.digitalInputAtAddress(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAtAddress(18)->value().valueBool(), false);
  // Check multiple digital inputs update - List contains to much items
  values.clear();
  values << false << true;
  ios.updateDigitalInputValues(values, -1, -1, false);
  QCOMPARE(ios.digitalInputAtAddress(17)->value().valueBool(), false);
  QCOMPARE(ios.digitalInputAtAddress(18)->value().valueBool(), true);
  values.clear();
  values << true << false;
  ios.updateDigitalInputValues(values, -1, 1, false);
  QCOMPARE(ios.digitalInputAtAddress(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAtAddress(18)->value().valueBool(), true);
  values.clear();
  values << false << false;
  ios.updateDigitalInputValues(values, 18, 1, false);
  QCOMPARE(ios.digitalInputAtAddress(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAtAddress(18)->value().valueBool(), false);
  // We add a new digital input with address 20 (we have also 17, 18, 20)
  di = new mdtDigitalIo;
  di->setAddress(20);
  di->setValue(false);
  ios.addDigitalInput(di);
  QCOMPARE(ios.digitalInputsCount(), 3);
  QVERIFY(ios.digitalInputAtAddress(17) != 0);
  QVERIFY(ios.digitalInputAtAddress(18) != 0);
  QVERIFY(ios.digitalInputAtAddress(20) != 0);
  QCOMPARE(ios.digitalInputAtAddress(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAtAddress(18)->value().valueBool(), false);
  QCOMPARE(ios.digitalInputAtAddress(20)->value().valueBool(), false);
  // Update address 18 and 20 - No address match - List contains to much items
  values.clear();
  values << true << true << true << true;
  ios.updateDigitalInputValues(values, 18, 2, false);
  QCOMPARE(ios.digitalInputAtAddress(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAtAddress(18)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAtAddress(20)->value().valueBool(), true);
  // Update address 18 - No address match - List contains to much items
  values.clear();
  values << false << false << false << false;
  ios.updateDigitalInputValues(values, 18, 1, false);
  QCOMPARE(ios.digitalInputAtAddress(17)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAtAddress(18)->value().valueBool(), false);
  QCOMPARE(ios.digitalInputAtAddress(20)->value().valueBool(), true);

  // Check digital outputs
  QVERIFY(ios.digitalOutputAtAddressRead(19) == 0);
  QVERIFY(ios.digitalOutputAtAddressRead(20) != 0);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->addressRead(), 20);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->addressWrite(), 120);
  QVERIFY(ios.digitalOutputAtAddressWrite(19) == 0);
  QVERIFY(ios.digitalOutputAtAddressWrite(120) != 0);
  QCOMPARE(ios.digitalOutputAtAddressWrite(120)->addressRead(), 20);
  QCOMPARE(ios.digitalOutputAtAddressWrite(120)->addressWrite(), 120);
  ios.digitalOutputAtAddressRead(20)->setLabelShort("K20");
  QVERIFY(ios.digitalOutputWithLabelShort("") == 0);
  QVERIFY(ios.digitalOutputWithLabelShort("K20") != 0);
  QCOMPARE(ios.digitalOutputWithLabelShort("K20")->addressRead(), 20);
  QCOMPARE(ios.digitalOutputWithLabelShort("K20")->addressWrite(), 120);
  QCOMPARE(ios.digitalOutputWithLabelShort("K20")->labelShort(), QString("K20"));
  QVERIFY(ios.digitalOutputAtAddressRead(30) != 0);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->addressRead(), 30);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->addressWrite(), 130);
  QVERIFY(ios.digitalOutputAtAddressWrite(130) != 0);
  QCOMPARE(ios.digitalOutputAtAddressWrite(130)->addressRead(), 30);
  QCOMPARE(ios.digitalOutputAtAddressWrite(130)->addressWrite(), 130);
  QVERIFY(ios.digitalOutputWithLabelShort("DO30") != 0);
  QCOMPARE(ios.digitalOutputWithLabelShort("DO30")->addressRead(), 30);
  QCOMPARE(ios.digitalOutputWithLabelShort("DO30")->addressWrite(), 130);
  QCOMPARE(ios.digitalOutputWithLabelShort("DO30")->labelShort(), QString("DO30"));
  dIoList = ios.digitalOutputs();
  QCOMPARE(dIoList.size(), 2);
  // Check multiple digital outputs update - List contains the correct number of items
  values.clear();
  values << false << true;
  ios.updateDigitalOutputValues(values, -1, mdtDeviceIos::Read, -1, false);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), false);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), true);
  values.clear();
  values << true;
  ios.updateDigitalOutputValues(values, -1, mdtDeviceIos::Read, 1, false);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), true);
  values.clear();
  values << false;
  ios.updateDigitalOutputValues(values, 30, mdtDeviceIos::Read, 1, false);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), false);
  // Check multiple digital outputs update - List contains the to much items
  values.clear();
  values << false << true;
  ios.updateDigitalOutputValues(values, -1, mdtDeviceIos::Read, -1, false);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), false);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), true);
  values.clear();
  values << true << false;
  ios.updateDigitalOutputValues(values, -1, mdtDeviceIos::Read, 1, false);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), true);
  values.clear();
  values << false << false;
  ios.updateDigitalOutputValues(values, 30, mdtDeviceIos::Read, 1, false);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), false);
  // We add a digital output with address read 29 / write 129 (so we have 20/120,29/129,30/130)
  dout = new mdtDigitalIo;
  dout->setAddressRead(29);
  dout->setAddressWrite(129);
  dout->setValue(false);
  ios.addDigitalOutput(dout);
  QCOMPARE(ios.digitalOutputsCount(), 3);
  QVERIFY(ios.digitalOutputAtAddressRead(20) != 0);
  QVERIFY(ios.digitalOutputAtAddressRead(29) != 0);
  QVERIFY(ios.digitalOutputAtAddressRead(30) != 0);
  QVERIFY(ios.digitalOutputAtAddressWrite(120) != 0);
  QVERIFY(ios.digitalOutputAtAddressWrite(129) != 0);
  QVERIFY(ios.digitalOutputAtAddressWrite(130) != 0);
  // Check multiple digital outputs update starting at address read 29, 1 item, with a list of 2 items
  values.clear();
  values << true << true;
  ios.updateDigitalOutputValues(values, 29, mdtDeviceIos::Read, 1, false);
  QCOMPARE(ios.digitalOutputAtAddressRead(20)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(29)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(30)->value().valueBool(), false);
  // Check multiple digital outputs update starting at address write 129, 1 item, with a list of 2 items


}

void mdtDeviceTest::deviceIosSegmentStorageTest()
{
  mdtDeviceIos ios;
  mdtAnalogIo *ai;
  mdtAnalogIo *ao;
  mdtDigitalIo *di;
  mdtDigitalIo *dout;
  mdtDeviceIosSegment *seg;
  QList<int> expectedValuesInt;
  QList<bool> expectedValuesBool;
  QList<QVariant> valuesVar;

  /*
   * Analog IN
   */

  // Add analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(0);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(2);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(1);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(5);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(6);
  ios.addAnalogInput(ai);
  // Check segments
  QCOMPARE(ios.analogInputsSegments().size(), 2);
  // First segment
  seg = ios.analogInputsSegments().at(0);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 3);
  QCOMPARE(seg->addressesRead().at(0), 0);
  QCOMPARE(seg->addressesRead().at(1), 1);
  QCOMPARE(seg->addressesRead().at(2), 2);
  // First segment - Set some values and check
  ai = ios.analogInputAtAddress(0);
  QVERIFY(ai != 0);
  ai->setValue(10);
  ai = ios.analogInputAtAddress(1);
  QVERIFY(ai != 0);
  ai->setValue(11);
  ai = ios.analogInputAtAddress(2);
  QVERIFY(ai != 0);
  ai->setValue(12);
  expectedValuesInt.clear();
  expectedValuesInt << 10 << 11 << 12;
  QCOMPARE(seg->valuesInt(), expectedValuesInt);
  // Second segment
  seg = ios.analogInputsSegments().at(1);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 2);
  QCOMPARE(seg->addressesRead().at(0), 5);
  QCOMPARE(seg->addressesRead().at(1), 6);
  // Second segment - Set some values and check
  ai = ios.analogInputAtAddress(5);
  QVERIFY(ai != 0);
  ai->setValue(15);
  ai = ios.analogInputAtAddress(6);
  QVERIFY(ai != 0);
  ai->setValue(16);
  expectedValuesInt.clear();
  expectedValuesInt << 15 << 16;
  QCOMPARE(seg->valuesInt(), expectedValuesInt);
  // Check grouped updates - Only first segment
  valuesVar.clear();
  valuesVar << -10;
  QVERIFY(ios.analogInputAtAddress(0) != 0);
  QVERIFY(ios.analogInputAtAddress(0)->value().valueInt() != -10);
  ios.updateAnalogInputValues(valuesVar, 0, -1, true);
  QCOMPARE(ios.analogInputAtAddress(0)->value().valueInt(), -10);
  ios.updateAnalogInputValues(valuesVar, 3, -1, true);  // Address 3 not exists
  QCOMPARE(ios.analogInputAtAddress(0)->value().valueInt(), -10);
  // Check grouped updates - first + second segments
  // Note: considere that we only included I/O's with addresses 0,1,2,5,6 in container, but we made a query to device for addresses 2-6
  valuesVar.clear();
  valuesVar << 200 << 300 << 400 << 500 << 600;
  QVERIFY(ios.analogInputAtAddress(2) != 0);
  QVERIFY(ios.analogInputAtAddress(2)->value().valueInt() != 200);
  QVERIFY(ios.analogInputAtAddress(5) != 0);
  QVERIFY(ios.analogInputAtAddress(5)->value().valueInt() != 500);
  QVERIFY(ios.analogInputAtAddress(6) != 0);
  QVERIFY(ios.analogInputAtAddress(6)->value().valueInt() != 600);
  ios.updateAnalogInputValues(valuesVar, 2, -1, true);
  QCOMPARE(ios.analogInputAtAddress(2)->value().valueInt(), 200);
  QCOMPARE(ios.analogInputAtAddress(5)->value().valueInt(), 500);
  QCOMPARE(ios.analogInputAtAddress(6)->value().valueInt(), 600);
  // Same story, but we queried device about addresses 0-3 (and I/O with address 3 does not exist in container)
  valuesVar.clear();
  valuesVar << 0 << 10 << 20 << 30;
  QVERIFY(ios.analogInputAtAddress(0) != 0);
  QVERIFY(ios.analogInputAtAddress(0)->value().valueInt() != 0);
  QVERIFY(ios.analogInputAtAddress(1) != 0);
  QVERIFY(ios.analogInputAtAddress(1)->value().valueInt() != 10);
  QVERIFY(ios.analogInputAtAddress(2) != 0);
  QVERIFY(ios.analogInputAtAddress(2)->value().valueInt() != 20);
  // We will check that 4th I/O was untouched
  QVERIFY(ios.analogInputAtAddress(5) != 0);
  QVERIFY(ios.analogInputAtAddress(5)->value().valueInt() == 500);
  ios.updateAnalogInputValues(valuesVar, -1, -1, true);
  QCOMPARE(ios.analogInputAtAddress(0)->value().valueInt(), 0);
  QCOMPARE(ios.analogInputAtAddress(1)->value().valueInt(), 10);
  QCOMPARE(ios.analogInputAtAddress(2)->value().valueInt(), 20);
  // Check that 4th I/O was untouched
  QCOMPARE(ios.analogInputAtAddress(5)->value().valueInt(), 500);

  /*
   * Analog OUT
   */

  // Add analog outputs - consider write address access
  ao = new mdtAnalogIo;
  ao->setAddressRead(1);
  ao->setAddressWrite(11);
  ios.addAnalogOutput(ao, mdtDeviceIos::Write);
  ao = new mdtAnalogIo;
  ao->setAddressRead(2);
  ao->setAddressWrite(12);
  ios.addAnalogOutput(ao, mdtDeviceIos::Write);
  ao = new mdtAnalogIo;
  ao->setAddressRead(7);
  ao->setAddressWrite(17);
  ios.addAnalogOutput(ao, mdtDeviceIos::Write);
  ao = new mdtAnalogIo;
  ao->setAddressRead(5);
  ao->setAddressWrite(15);
  ios.addAnalogOutput(ao, mdtDeviceIos::Write);
  ao = new mdtAnalogIo;
  ao->setAddressRead(6);
  ao->setAddressWrite(16);
  ios.addAnalogOutput(ao, mdtDeviceIos::Write);
  // Check segments
  QCOMPARE(ios.analogOutputsSegments().size(), 2);
  // First segment
  seg = ios.analogOutputsSegments().at(0);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 2);
  QCOMPARE(seg->addressesRead().at(0), 1);
  QCOMPARE(seg->addressesRead().at(1), 2);
  QCOMPARE(seg->addressesWrite().size(), 2);
  QCOMPARE(seg->addressesWrite().at(0), 11);
  QCOMPARE(seg->addressesWrite().at(1), 12);
  // Second segment
  seg = ios.analogOutputsSegments().at(1);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 3);
  QCOMPARE(seg->addressesRead().at(0), 5);
  QCOMPARE(seg->addressesRead().at(1), 6);
  QCOMPARE(seg->addressesRead().at(1), 6);
  QCOMPARE(seg->addressesWrite().size(), 3);
  QCOMPARE(seg->addressesWrite().at(0), 15);
  QCOMPARE(seg->addressesWrite().at(1), 16);
  QCOMPARE(seg->addressesWrite().at(2), 17);
  // Check delete
  ios.deleteIos();
  QCOMPARE(ios.analogOutputsSegments().size(), 0);
  // Add analog outputs - consider read address access
  ao = new mdtAnalogIo;
  ao->setAddressRead(1);
  ao->setAddressWrite(11);
  ios.addAnalogOutput(ao, mdtDeviceIos::Read);
  ao = new mdtAnalogIo;
  ao->setAddressRead(2);
  ao->setAddressWrite(12);
  ios.addAnalogOutput(ao, mdtDeviceIos::Read);
  ao = new mdtAnalogIo;
  ao->setAddressRead(7);
  ao->setAddressWrite(17);
  ios.addAnalogOutput(ao, mdtDeviceIos::Read);
  ao = new mdtAnalogIo;
  ao->setAddressRead(5);
  ao->setAddressWrite(15);
  ios.addAnalogOutput(ao, mdtDeviceIos::Read);
  ao = new mdtAnalogIo;
  ao->setAddressRead(6);
  ao->setAddressWrite(16);
  ios.addAnalogOutput(ao, mdtDeviceIos::Read);
  // Check segments
  QCOMPARE(ios.analogOutputsSegments().size(), 2);
  // First segment
  seg = ios.analogOutputsSegments().at(0);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 2);
  QCOMPARE(seg->addressesRead().at(0), 1);
  QCOMPARE(seg->addressesRead().at(1), 2);
  QCOMPARE(seg->addressesWrite().size(), 2);
  QCOMPARE(seg->addressesWrite().at(0), 11);
  QCOMPARE(seg->addressesWrite().at(1), 12);
  // Second segment
  seg = ios.analogOutputsSegments().at(1);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 3);
  QCOMPARE(seg->addressesRead().at(0), 5);
  QCOMPARE(seg->addressesRead().at(1), 6);
  QCOMPARE(seg->addressesRead().at(1), 6);
  QCOMPARE(seg->addressesWrite().size(), 3);
  QCOMPARE(seg->addressesWrite().at(0), 15);
  QCOMPARE(seg->addressesWrite().at(1), 16);
  QCOMPARE(seg->addressesWrite().at(2), 17);
  // First segment - Set some values and check
  ao = ios.analogOutputAtAddressRead(1);
  QVERIFY(ao != 0);
  ao->setValue(100);
  ao = ios.analogOutputAtAddressRead(2);
  QVERIFY(ao != 0);
  ao->setValue(200);
  expectedValuesInt.clear();
  expectedValuesInt << 100 << 200;
  seg = ios.analogOutputsSegments().at(0);
  QVERIFY(seg != 0);
  QCOMPARE(seg->valuesInt(), expectedValuesInt);
  // Second segment - Set some values and check
  ao = ios.analogOutputAtAddressRead(5);
  QVERIFY(ao != 0);
  ao->setValue(50);
  ao = ios.analogOutputAtAddressRead(6);
  QVERIFY(ao != 0);
  ao->setValue(60);
  ao = ios.analogOutputAtAddressRead(7);
  QVERIFY(ao != 0);
  ao->setValue(70);
  expectedValuesInt.clear();
  expectedValuesInt << 50 << 60 << 70;
  seg = ios.analogOutputsSegments().at(1);
  QVERIFY(seg != 0);
  QCOMPARE(seg->valuesInt(), expectedValuesInt);
  // Check grouped updates - Only first segment
  valuesVar.clear();
  valuesVar << -10 << -20;  // Supposed addresses for read access 1 and 2
  QVERIFY(ios.analogOutputAtAddressRead(1) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(1)->value().valueInt() != -10);
  QVERIFY(ios.analogOutputAtAddressRead(2) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(2)->value().valueInt() != -20);
  ios.updateAnalogOutputValues(valuesVar, 1, mdtDeviceIos::Read, -1, true);
  QCOMPARE(ios.analogOutputAtAddressRead(1)->value().valueInt(), -10);
  QCOMPARE(ios.analogOutputAtAddressRead(2)->value().valueInt(), -20);
  ios.updateAnalogOutputValues(valuesVar, 3, mdtDeviceIos::Read, -1, true);  // Address 3 not exists
  QCOMPARE(ios.analogOutputAtAddressRead(1)->value().valueInt(), -10);
  QCOMPARE(ios.analogOutputAtAddressRead(2)->value().valueInt(), -20);
  valuesVar.clear();
  valuesVar << -11 << -12;  // Supposed addresses for write access 11 and 12
  QVERIFY(ios.analogOutputAtAddressRead(1)->value().valueInt() != -11);
  QVERIFY(ios.analogOutputAtAddressRead(2)->value().valueInt() != -12);
  ios.updateAnalogOutputValues(valuesVar, 11, mdtDeviceIos::Write, -1, true);
  QCOMPARE(ios.analogOutputAtAddressRead(1)->value().valueInt(), -11);
  QCOMPARE(ios.analogOutputAtAddressRead(2)->value().valueInt(), -12);
  ios.updateAnalogOutputValues(valuesVar, 13, mdtDeviceIos::Write, -1, true);  // Address 13 not exists
  QCOMPARE(ios.analogOutputAtAddressRead(1)->value().valueInt(), -11);
  QCOMPARE(ios.analogOutputAtAddressRead(2)->value().valueInt(), -12);
  // Check grouped updates - first + second segments
  // Note: considere that we only included I/O's with addresses 1,2,5,6,7 in container, but we made a query to device for addresses 2-6
  valuesVar.clear();
  valuesVar << 200 << 300 << 400 << 500 << 600;
  QVERIFY(ios.analogOutputAtAddressRead(2) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(2)->value().valueInt() != 200);
  QVERIFY(ios.analogOutputAtAddressRead(5) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(5)->value().valueInt() != 500);
  QVERIFY(ios.analogOutputAtAddressRead(6) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(6)->value().valueInt() != 600);
  ios.updateAnalogOutputValues(valuesVar, 2, mdtDeviceIos::Read, -1, true);
  QCOMPARE(ios.analogOutputAtAddressRead(2)->value().valueInt(), 200);
  QCOMPARE(ios.analogOutputAtAddressRead(5)->value().valueInt(), 500);
  QCOMPARE(ios.analogOutputAtAddressRead(6)->value().valueInt(), 600);
  // Set a known value at read address 7 for next test
  QVERIFY(ios.analogOutputAtAddressRead(7) != 0);
  ios.analogOutputAtAddressRead(7)->setValue(70);
  // Same story, but we queried device about addresses 1-6 (I/O with addresses 0, 3 and 4 does not exist in container)
  valuesVar.clear();
  valuesVar << 10 << 20 << 30 << 40 << 50 << 60;
  QVERIFY(ios.analogOutputAtAddressRead(1) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(1)->value().valueInt() != 10);
  QVERIFY(ios.analogOutputAtAddressRead(2) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(2)->value().valueInt() != 20);
  QVERIFY(ios.analogOutputAtAddressRead(5) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(5)->value().valueInt() != 50);
  QVERIFY(ios.analogOutputAtAddressRead(6) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(6)->value().valueInt() != 60);
  // We will check that I/O with read address 7 was untouched
  QVERIFY(ios.analogOutputAtAddressRead(7) != 0);
  QVERIFY(ios.analogOutputAtAddressRead(7)->value().valueInt() == 70);
  // Update I/O's and check
  ios.updateAnalogOutputValues(valuesVar, -1, mdtDeviceIos::Read, -1, true);
  QCOMPARE(ios.analogOutputAtAddressRead(1)->value().valueInt(), 10);
  QCOMPARE(ios.analogOutputAtAddressRead(2)->value().valueInt(), 20);
  QCOMPARE(ios.analogOutputAtAddressRead(5)->value().valueInt(), 50);
  QCOMPARE(ios.analogOutputAtAddressRead(6)->value().valueInt(), 60);
  // Check that I/O with read address 7 was untouched
  QCOMPARE(ios.analogOutputAtAddressRead(7)->value().valueInt(), 70);

  /*
   * Digital IN
   */

  // Add digital inputs
  di = new mdtDigitalIo;
  di->setAddress(0);
  ios.addDigitalInput(di);
  di = new mdtDigitalIo;
  di->setAddress(2);
  ios.addDigitalInput(di);
  di = new mdtDigitalIo;
  di->setAddress(1);
  ios.addDigitalInput(di);
  di = new mdtDigitalIo;
  di->setAddress(5);
  ios.addDigitalInput(di);
  di = new mdtDigitalIo;
  di->setAddress(6);
  ios.addDigitalInput(di);
  // Check segments
  QCOMPARE(ios.digitalInputsSegments().size(), 2);
  // First segment
  seg = ios.digitalInputsSegments().at(0);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 3);
  QCOMPARE(seg->addressesRead().at(0), 0);
  QCOMPARE(seg->addressesRead().at(1), 1);
  QCOMPARE(seg->addressesRead().at(2), 2);
  // First segment - Set some values and check
  di = ios.digitalInputAtAddress(0);
  QVERIFY(di != 0);
  di->setValue(true);
  di = ios.digitalInputAtAddress(1);
  QVERIFY(di != 0);
  di->setValue(false);
  di = ios.digitalInputAtAddress(2);
  QVERIFY(di != 0);
  di->setValue(true);
  expectedValuesBool.clear();
  expectedValuesBool << true << false << true;
  QCOMPARE(seg->valuesBool(), expectedValuesBool);
  // Second segment
  seg = ios.digitalInputsSegments().at(1);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 2);
  QCOMPARE(seg->addressesRead().at(0), 5);
  QCOMPARE(seg->addressesRead().at(1), 6);
  // Second segment - Set some values and check
  di = ios.digitalInputAtAddress(5);
  QVERIFY(di != 0);
  di->setValue(true);
  di = ios.digitalInputAtAddress(6);
  QVERIFY(di != 0);
  di->setValue(false);
  expectedValuesBool.clear();
  expectedValuesBool << true << false;
  QCOMPARE(seg->valuesBool(), expectedValuesBool);
  // Check grouped updates - Only first segment
  valuesVar.clear();
  valuesVar << false;
  QVERIFY(ios.digitalInputAtAddress(0) != 0);
  QVERIFY(ios.digitalInputAtAddress(0)->value().valueBool() != false);
  ios.updateDigitalInputValues(valuesVar, 0, -1, true);
  QCOMPARE(ios.digitalInputAtAddress(0)->value().valueBool(), false);
  ios.updateDigitalInputValues(valuesVar, 3, -1, true);  // Address 3 not exists
  QCOMPARE(ios.digitalInputAtAddress(0)->value().valueBool(), false);
  // Check grouped updates - first + second segments
  // Note: considere that we only included I/O's with addresses 0,1,2,5,6 in container, but we made a query to device for addresses 2-6
  valuesVar.clear();
  valuesVar << true << false << true << false << true;  // Data for device addresses 2, 3, 4, 5 and 6
  ios.setDigitalInputsValue(mdtValue());
  QVERIFY(ios.digitalInputAtAddress(0) != 0);
  QVERIFY(ios.digitalInputAtAddress(1) != 0);
  QVERIFY(ios.digitalInputAtAddress(2) != 0);
  QVERIFY(ios.digitalInputAtAddress(5) != 0);
  QVERIFY(ios.digitalInputAtAddress(6) != 0);
  ios.updateDigitalInputValues(valuesVar, 2, -1, true);
  QCOMPARE(ios.digitalInputAtAddress(2)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAtAddress(5)->value().valueBool(), false);
  QCOMPARE(ios.digitalInputAtAddress(6)->value().valueBool(), true);
  // Same story, but we queried device about addresses 0-3 (and I/O with address 3 does not exist in container)
  valuesVar.clear();
  valuesVar << true << false << true << false;  // Data for device addresses 0, 1, 2 and 3
  ios.setDigitalInputsValue(mdtValue());
  // We will check that 4th I/O was untouched
  QVERIFY(ios.digitalInputAtAddress(5) != 0);
  QVERIFY(ios.digitalInputAtAddress(5)->value().valueBool() != true);
  ios.updateDigitalInputValues(valuesVar, -1, -1, true);
  QCOMPARE(ios.digitalInputAtAddress(0)->value().valueBool(), true);
  QCOMPARE(ios.digitalInputAtAddress(1)->value().valueBool(), false);
  QCOMPARE(ios.digitalInputAtAddress(2)->value().valueBool(), true);
  // Check that 4th I/O was untouched
  QCOMPARE(ios.digitalInputAtAddress(5)->value().valueBool(), false);

  /*
   * Digital OUT
   */

  // Add digital outputs - consider write address access
  dout = new mdtDigitalIo;
  dout->setAddressRead(1);
  dout->setAddressWrite(11);
  ios.addDigitalOutput(dout, mdtDeviceIos::Write);
  dout = new mdtDigitalIo;
  dout->setAddressRead(2);
  dout->setAddressWrite(12);
  ios.addDigitalOutput(dout, mdtDeviceIos::Write);
  dout = new mdtDigitalIo;
  dout->setAddressRead(7);
  dout->setAddressWrite(17);
  ios.addDigitalOutput(dout, mdtDeviceIos::Write);
  dout = new mdtDigitalIo;
  dout->setAddressRead(5);
  dout->setAddressWrite(15);
  ios.addDigitalOutput(dout, mdtDeviceIos::Write);
  dout = new mdtDigitalIo;
  dout->setAddressRead(6);
  dout->setAddressWrite(16);
  ios.addDigitalOutput(dout, mdtDeviceIos::Write);
  // Check segments
  QCOMPARE(ios.digitalOutputsSegments().size(), 2);
  // First segment
  seg = ios.digitalOutputsSegments().at(0);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 2);
  QCOMPARE(seg->addressesRead().at(0), 1);
  QCOMPARE(seg->addressesRead().at(1), 2);
  QCOMPARE(seg->addressesWrite().size(), 2);
  QCOMPARE(seg->addressesWrite().at(0), 11);
  QCOMPARE(seg->addressesWrite().at(1), 12);
  // Second segment
  seg = ios.digitalOutputsSegments().at(1);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 3);
  QCOMPARE(seg->addressesRead().at(0), 5);
  QCOMPARE(seg->addressesRead().at(1), 6);
  QCOMPARE(seg->addressesRead().at(1), 6);
  QCOMPARE(seg->addressesWrite().size(), 3);
  QCOMPARE(seg->addressesWrite().at(0), 15);
  QCOMPARE(seg->addressesWrite().at(1), 16);
  QCOMPARE(seg->addressesWrite().at(2), 17);
  // Check delete
  ios.deleteIos();
  QCOMPARE(ios.digitalOutputsSegments().size(), 0);
  // Add digital outputs - consider read address access
  dout = new mdtDigitalIo;
  dout->setAddressRead(1);
  dout->setAddressWrite(11);
  ios.addDigitalOutput(dout, mdtDeviceIos::Read);
  dout = new mdtDigitalIo;
  dout->setAddressRead(2);
  dout->setAddressWrite(12);
  ios.addDigitalOutput(dout, mdtDeviceIos::Read);
  dout = new mdtDigitalIo;
  dout->setAddressRead(7);
  dout->setAddressWrite(17);
  ios.addDigitalOutput(dout, mdtDeviceIos::Read);
  dout = new mdtDigitalIo;
  dout->setAddressRead(5);
  dout->setAddressWrite(15);
  ios.addDigitalOutput(dout, mdtDeviceIos::Read);
  dout = new mdtDigitalIo;
  dout->setAddressRead(6);
  dout->setAddressWrite(16);
  ios.addDigitalOutput(dout, mdtDeviceIos::Read);
  // Check segments
  QCOMPARE(ios.digitalOutputsSegments().size(), 2);
  // First segment
  seg = ios.digitalOutputsSegments().at(0);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 2);
  QCOMPARE(seg->addressesRead().at(0), 1);
  QCOMPARE(seg->addressesRead().at(1), 2);
  QCOMPARE(seg->addressesWrite().size(), 2);
  QCOMPARE(seg->addressesWrite().at(0), 11);
  QCOMPARE(seg->addressesWrite().at(1), 12);
  // Second segment
  seg = ios.digitalOutputsSegments().at(1);
  QVERIFY(seg != 0);
  QCOMPARE(seg->addressesRead().size(), 3);
  QCOMPARE(seg->addressesRead().at(0), 5);
  QCOMPARE(seg->addressesRead().at(1), 6);
  QCOMPARE(seg->addressesRead().at(1), 6);
  QCOMPARE(seg->addressesWrite().size(), 3);
  QCOMPARE(seg->addressesWrite().at(0), 15);
  QCOMPARE(seg->addressesWrite().at(1), 16);
  QCOMPARE(seg->addressesWrite().at(2), 17);
  // First segment - Set some values and check
  dout = ios.digitalOutputAtAddressRead(1);
  QVERIFY(dout != 0);
  dout->setValue(true);
  dout = ios.digitalOutputAtAddressRead(2);
  QVERIFY(dout != 0);
  dout->setValue(false);
  expectedValuesBool.clear();
  expectedValuesBool << true << false;
  seg = ios.digitalOutputsSegments().at(0);
  QVERIFY(seg != 0);
  QCOMPARE(seg->valuesBool(), expectedValuesBool);
  // Second segment - Set some values and check
  dout = ios.digitalOutputAtAddressRead(5);
  QVERIFY(dout != 0);
  dout->setValue(true);
  dout = ios.digitalOutputAtAddressRead(6);
  QVERIFY(dout != 0);
  dout->setValue(false);
  dout = ios.digitalOutputAtAddressRead(7);
  QVERIFY(dout != 0);
  dout->setValue(true);
  expectedValuesBool.clear();
  expectedValuesBool << true << false << true;
  seg = ios.digitalOutputsSegments().at(1);
  QVERIFY(seg != 0);
  QCOMPARE(seg->valuesBool(), expectedValuesBool);
  // Check that all I/O's was set
  QVERIFY(ios.digitalOutputAtAddressRead(1) != 0);
  QVERIFY(ios.digitalOutputAtAddressRead(2) != 0);
  QVERIFY(ios.digitalOutputAtAddressRead(5) != 0);
  QVERIFY(ios.digitalOutputAtAddressRead(6) != 0);
  QVERIFY(ios.digitalOutputAtAddressRead(7) != 0);
  QVERIFY(ios.digitalOutputAtAddressWrite(11) != 0);
  QVERIFY(ios.digitalOutputAtAddressWrite(12) != 0);
  QVERIFY(ios.digitalOutputAtAddressWrite(15) != 0);
  QVERIFY(ios.digitalOutputAtAddressWrite(16) != 0);
  QVERIFY(ios.digitalOutputAtAddressWrite(17) != 0);
  // Check grouped updates - Only first segment
  valuesVar.clear();
  valuesVar << false << true;  // Supposed addresses for read access 1 and 2
  QVERIFY(ios.digitalOutputAtAddressRead(1)->value().valueBool() != false);
  QVERIFY(ios.digitalOutputAtAddressRead(2)->value().valueBool() != true);
  ios.updateDigitalOutputValues(valuesVar, 1, mdtDeviceIos::Read, -1, true);
  QCOMPARE(ios.digitalOutputAtAddressRead(1)->value().valueBool(), false);
  QCOMPARE(ios.digitalOutputAtAddressRead(2)->value().valueBool(), true);
  ios.updateDigitalOutputValues(valuesVar, 3, mdtDeviceIos::Read, -1, true);  // Address 3 not exists
  QCOMPARE(ios.digitalOutputAtAddressRead(1)->value().valueBool(), false);
  QCOMPARE(ios.digitalOutputAtAddressRead(2)->value().valueBool(), true);
  valuesVar.clear();
  valuesVar << true << false;  // Supposed addresses for write access 11 and 12
  ios.updateDigitalOutputValues(valuesVar, 11, mdtDeviceIos::Write, -1, true);
  QCOMPARE(ios.digitalOutputAtAddressRead(1)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(2)->value().valueBool(), false);
  ios.updateDigitalOutputValues(valuesVar, 13, mdtDeviceIos::Write, -1, true);  // Address 13 not exists
  QCOMPARE(ios.digitalOutputAtAddressRead(1)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(2)->value().valueBool(), false);
  // Check grouped updates - first + second segments
  // Note: considere that we only included I/O's with addresses 1,2,5,6,7 in container, but we made a query to device for addresses 2-6
  ios.setDigitalOutputsValue(mdtValue());
  valuesVar.clear();
  valuesVar << true << false << true << false << true;
  ios.updateDigitalOutputValues(valuesVar, 2, mdtDeviceIos::Read, -1, true);
  QCOMPARE(ios.digitalOutputAtAddressRead(2)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(5)->value().valueBool(), false);
  QCOMPARE(ios.digitalOutputAtAddressRead(6)->value().valueBool(), true);
  // Set a known value at read address 7 for next test
  QVERIFY(ios.digitalOutputAtAddressRead(7) != 0);
  ios.digitalOutputAtAddressRead(7)->setValue(false);
  // Same story, but we queried device about addresses 1-6 (I/O with addresses 0, 3 and 4 does not exist in container)
  valuesVar.clear();
  valuesVar << false << true << false << true << false << true;
  // We will check that I/O with read address 7 was untouched
  QVERIFY(ios.digitalOutputAtAddressRead(7) != 0);
  QVERIFY(ios.digitalOutputAtAddressRead(7)->value().valueBool() == false);
  // Update I/O's and check
  ios.updateDigitalOutputValues(valuesVar, -1, mdtDeviceIos::Read, -1, true);
  QCOMPARE(ios.digitalOutputAtAddressRead(1)->value().valueBool(), false);
  QCOMPARE(ios.digitalOutputAtAddressRead(2)->value().valueBool(), true);
  QCOMPARE(ios.digitalOutputAtAddressRead(5)->value().valueBool(), false);
  QCOMPARE(ios.digitalOutputAtAddressRead(6)->value().valueBool(), true);
  // Check that I/O with read address 7 was untouched
  QCOMPARE(ios.digitalOutputAtAddressRead(7)->value().valueBool(), false);
}

void mdtDeviceTest::deviceIosWidgetTest()
{
  mdtDeviceIosWidget w;
  std::shared_ptr<mdtDeviceIos> ios(new mdtDeviceIos);
  mdtAnalogIo *ai;
  mdtAnalogIo *ao;
  mdtDigitalIo *di;
  mdtDigitalIo *dout;

  // Add analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(12);
  ai->setLabelShort("Vin");
  ai->setLabel("Input voltage");
  ai->setDetails("Input voltage measured from AG1\nSee schema 15G");
  ///QVERIFY(ai->setRange(0.0, 10.0, 16));
  ai->setRange(0.0, 10.0, 16);
  ///ios.addAnalogInput(ai);

  ai = new mdtAnalogIo;
  ai->setAddress(13);
  ai->setLabelShort("wX_BatUBa");
  ai->setLabel("rBatSpannung");
  ai->setDetails("Batteriespannung\nSchema: 06E");
  ///QVERIFY(ai->setRange(0.0, 50.0, 16));
  ai->setRange(0.0, 50.0, 16);
  ios->addAnalogInput(ai);

  // Add analog outputs
  ao = new mdtAnalogIo;
  ao->setAddress(15);
  ao->setLabelShort("Vout");
  ao->setLabel("Output voltage for AG speed");
  ao->setDetails("Output voltage for AG speed setpoint\nSee schema 16F");
  ///QVERIFY(ao->setRange(0.0, 10.0, 16));
  ao->setRange(0.0, 10.0, 16);
  ios->addAnalogOutput(ao);

  // Add digital inputs
  di = new mdtDigitalIo;
  di->setAddress(17);
  di->setLabelShort("Test");
  di->setLabel("Test mode");
  di->setDetails("Test mode enabled.\nIf ON, it's possible to set some outputs from tool\nSee schema 25G");
  ios->addDigitalInput(di);

  di = new mdtDigitalIo;
  di->setAddress(18);
  di->setLabelShort("xM_HSEin");
  di->setLabel(tr("xHochspannung"));
  di->setDetails("Hoschspannung vorhanden\nSchema: 06E");
  ios->addDigitalInput(di);

  di = new mdtDigitalIo;
  di->setAddress(19);
  di->setLabelShort("xM_MgBr");
  di->setLabel(tr("xMgBremse"));
  di->setDetails("Magnetische Bremse\nSchema: 06E");
  ios->addDigitalInput(di);

  // Add digital outputs
  dout = new mdtDigitalIo;
  dout->setAddress(20);
  dout->setLabelShort("xB_BelEin");
  dout->setLabel(tr("xBeleuchtungEin"));
  dout->setDetails(tr("Drehschalter Beleuchtung Ein") + "\n" + tr("Schema: 10A"));
  ios->addDigitalOutput(dout);

  dout = new mdtDigitalIo;
  dout->setAddress(21);
  dout->setLabelShort("xB_BatMinU");
  dout->setLabel(tr("xBatMinSpg"));
  dout->setDetails(tr("Batterie Minimalspannung") + "\n" + tr("Schema: 06C"));
  ios->addDigitalOutput(dout);

  // Setup widget
  ///w.setMaxColumns(3);
  w.setDeviceIos(ios);
  w.show();

  /**
  while(w.isVisible()){
    QTest::qWait(1000);
  }
  */
  // Second setup (must clear existing widget and redo layout)
  w.setDeviceIos(ios);
  w.show();
  /**
  while(w.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDeviceTest::multiIoDeviceTest()
{
  mdtMultiIoDevice device;

  // Initial state
  QVERIFY(device.ios().get() != nullptr);
  
  QFAIL("Not implemented yet");
}

void mdtDeviceTest::modbusTest()
{
  mdtDeviceModbus d;
  mdtModbusTcpPortManager *m = d.modbusTcpPortManager();
  QString host;
  mdtDeviceAddress da;
  mdtDeviceAddressList daList;
  mdtDeviceIosWidget *iosw;
  mdtDigitalIo *di;
  mdtDeviceWindow dw;

  QVERIFY(m != 0);

  /*
   * Setup some digital inputs (they are not device dependent)
   */
  di = new mdtDigitalIo;
  di->setAddress(0);
  di->setLabelShort("DI1");
  di->setDetails("Some details about digital input 1");
  d.addInput(di);
  di = new mdtDigitalIo;
  di->setAddress(1);
  di->setLabelShort("DI2");
  di->setDetails("Some details about digital input 2");
  d.addInput(di);

  /*
   * Setup I/O's widget
   */
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(d.ios());

  /*
   * Setup device window
   */
  dw.setDevice(&d);
  dw.setIosWidget(iosw);
  dw.show();

  /*
   * Check connectToDevice() by giving a known device address.
   */
  // Scan network
  daList = m->scan(QNetworkInterface::allInterfaces(), 502, true, 50);
  if(daList.isEmpty()){
    QSKIP("No MODBUS/TCP device found on network (or other error)", SkipAll);
  }
  QVERIFY(daList.size() > 0);
  /*
   * Check setDeviceAddress() and connectToDevice()
   *  -> Case with host and port given
   */
  // Get first device found during scan
  da = daList.at(0);
  host = da.tcpIpHostName();
  QVERIFY(!host.isEmpty());
  // Build device address
  da.setModbusTcpIdentification(host, 502, mdtModbusHwNodeId());
  da.setAlias("NodeA");
  // Connect to device
  d.setDeviceAddress(da);
  QVERIFY(d.connectToDevice());
  QVERIFY(d.currentState() == mdtDevice::State_t::Ready);
  QCOMPARE(d.alias(), QString("NodeA"));
  // Disconnect
  d.disconnectFromDevice();
  QVERIFY(d.currentState() == mdtDevice::State_t::Disconnected);
  /*
   * We assume that we have 2 digital inputs, that are all OFF
   * Check setDeviceAddress() and connectToDevice()
   *  -> Case with host, port and MODBUS HW node ID 0 given
   */
  // Update device address
  da.setModbusTcpIdentification(host, 502, mdtModbusHwNodeId(0, 2, 0));
  da.setAlias("NodeB");
  d.setDeviceAddress(da);
  // Connect to device
  QVERIFY(d.connectToDevice());
  QVERIFY(d.currentState() == mdtDevice::State_t::Ready);
  QCOMPARE(d.alias(), QString("NodeB"));
  // Disconnect
  d.disconnectFromDevice();
  QVERIFY(d.currentState() == mdtDevice::State_t::Disconnected);
  /*
   * Check setDeviceAddress() and connectToDevice()
   *  -> Case with same host than above,  but giving a wrong MODBUS HW node ID
   */
  // Update device address
  da.setModbusTcpIdentification(host, 502, mdtModbusHwNodeId(1, 2, 0));
  da.setAlias("NodeB2");
  d.setDeviceAddress(da);
  // Connect to device
  QVERIFY(!d.connectToDevice());
  QVERIFY(d.currentState() == mdtDevice::State_t::Disconnected);
  /*
   * Try to connect to first device that has hardware node ID 0
   */
  QVERIFY(d.connectToDevice(mdtModbusHwNodeId(0,2,0), "NodeC", 100));
  QVERIFY(d.currentState() == mdtDevice::State_t::Ready);
  QCOMPARE(d.alias(), QString("NodeC"));
  /*
   * Do above a second time
   * (There was a bug when cache file existed)
   */
  // Disconnect
  d.disconnectFromDevice();
  QVERIFY(d.currentState() == mdtDevice::State_t::Disconnected);
  // Connect to first device with hardware node ID 0
  QVERIFY(d.connectToDevice(mdtModbusHwNodeId(0,2,0), "NodeD", 100));
  QVERIFY(d.currentState() == mdtDevice::State_t::Ready);
  QCOMPARE(d.alias(), QString("NodeD"));
  // Disconnect
  d.disconnectFromDevice();
  QVERIFY(d.currentState() == mdtDevice::State_t::Disconnected);
  /*
   * Try to connect to first device that has hardware node ID 3, assuming it not exists on network
   */
  QVERIFY(!d.connectToDevice(mdtModbusHwNodeId(3,2,0), "NodeD2", 100));
  QVERIFY(d.currentState() == mdtDevice::State_t::Disconnected);
  /*
   * Check setDeviceAddress() and connectToDevice()
   *  -> Case with only MODBUS HW node ID given
   */
  // Update device address
  da.setModbusTcpIdentification("", 0, mdtModbusHwNodeId(0, 2, 0));
  da.setAlias("NodeE");
  d.setDeviceAddress(da);
  // Connect to device
  QVERIFY(d.connectToDevice());
  QVERIFY(d.currentState() == mdtDevice::State_t::Ready);
  QCOMPARE(d.alias(), QString("NodeE"));
  /*
   * Make some queries
   * Note: we cannot detect I/O's, so we can just assume we have at least 2 digital inputs
   */
  ///QVERIFY(!d.getDigitalInputValue(0, false, false).isValid());
  QVERIFY(d.getDigitalInputValue(0, true, true).isValid());
  QVERIFY(d.getDigitalInputValue(0, false, false).isValid());
  QVERIFY(!d.getDigitalInputValue(0, true, false).isValid());
  ///QVERIFY(!d.getDigitalInputState(0, 0).isValid());
  QVERIFY(d.getDigitalOutputs(500) >= 0);
  d.start(100);
  QTest::qWait(1000);
  /*
  while(dw.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDeviceTest::modbusWagoModuleTest()
{
  mdtDeviceModbusWago *device = new mdtDeviceModbusWago;
  mdtDeviceModbusWagoModule m(true, device);
  mdtAnalogIo *aio;
  mdtDigitalIo *dio;

  // Check initial values
  QCOMPARE(m.firstAiAddress() , 0);
  QCOMPARE(m.lastAiAddress() , 0);
  QCOMPARE(m.nextModuleFirstAiAddress() , 0);
  QCOMPARE(m.firstAoAddressRead() , 0);
  QCOMPARE(m.lastAoAddressRead() , 0);
  QCOMPARE(m.nextModuleFirstAoAddressRead() , 0);
  QCOMPARE(m.firstAoAddressWrite() , 0);
  QCOMPARE(m.lastAoAddressWrite() , 0);
  QCOMPARE(m.nextModuleFirstAoAddressWrite() , 0);
  QVERIFY(m.analogInputs().isEmpty());
  QVERIFY(m.analogOutputs().isEmpty());
  ///QVERIFY(m.digitalIos().isEmpty());
  QVERIFY(m.digitalInputs().isEmpty());
  QVERIFY(m.digitalOutputs().isEmpty());
  QVERIFY(m.type() == mdtDeviceModbusWagoModule::Unknown);
  QCOMPARE(m.partNumber() , 0);
  QVERIFY(m.partNumberText().isEmpty());

  /*
   * Analog IN module
   *
   * Check with 750-457 witch has:
   *  - 4 analog IN
   *  - Range: -10V to +10V
   *  - Unit: [V]
   *
   * We use a address range:
   *  - Read access: 2-5
   *  - Write access: N/A
   */
  QVERIFY(m.setupFromRegisterWord(457));
  ///m.setFirstAddress(2);
  m.updateAddresses(2, 0, 0, 0, 0, 0);
  QCOMPARE(m.firstAiAddress() , 2);
  QCOMPARE(m.lastAiAddress() , 5);
  QCOMPARE(m.nextModuleFirstAiAddress() , 6);
  QCOMPARE(m.firstAoAddressRead() , 0);
  QCOMPARE(m.lastAoAddressRead() , 0);
  QCOMPARE(m.nextModuleFirstAoAddressRead() , 0);
  QCOMPARE(m.firstAoAddressWrite() , 0);
  QCOMPARE(m.lastAoAddressWrite() , 0);
  QCOMPARE(m.nextModuleFirstAoAddressWrite() , 0);
  ///QCOMPARE(m.ioCount() , 4);
  QCOMPARE(m.analogInputs().size(), 4);
  QCOMPARE(m.analogOutputs().size(), 0);
  ///QVERIFY(m.digitalIos().isEmpty());
  QVERIFY(m.digitalInputs().isEmpty());
  QVERIFY(m.digitalOutputs().isEmpty());
  QVERIFY(m.type() == mdtDeviceModbusWagoModule::AnalogInputs);
  QCOMPARE(m.partNumber() , 457);
  QCOMPARE(m.partNumberText() , QString("457"));
  // Check internal I/O setup
  aio = m.analogInputs().at(0);
  QVERIFY(aio != 0);
  MDT_COMPARE(aio->minimum(), -10.0, 12, -10.0, 10.0);
  MDT_COMPARE(aio->maximum(), 10.0, 12, -10.0, 10.0);
  QCOMPARE(aio->unit(), QString("[V]"));
  QCOMPARE(aio->addressRead(), 2);
  QVERIFY(aio->addressWrite() == aio->addressRead());
  aio = m.analogInputs().at(1);
  QVERIFY(aio != 0);
  MDT_COMPARE(aio->minimum(), -10.0, 12, -10.0, 10.0);
  MDT_COMPARE(aio->maximum(), 10.0, 12, -10.0, 10.0);
  QCOMPARE(aio->unit(), QString("[V]"));
  QCOMPARE(aio->addressRead(), 3);
  QVERIFY(aio->addressWrite() == aio->addressRead());
  aio = m.analogInputs().at(2);
  QVERIFY(aio != 0);
  MDT_COMPARE(aio->minimum(), -10.0, 12, -10.0, 10.0);
  MDT_COMPARE(aio->maximum(), 10.0, 12, -10.0, 10.0);
  QCOMPARE(aio->unit(), QString("[V]"));
  QCOMPARE(aio->addressRead(), 4);
  QVERIFY(aio->addressWrite() == aio->addressRead());
  aio = m.analogInputs().at(3);
  QVERIFY(aio != 0);
  MDT_COMPARE(aio->minimum(), -10.0, 12, -10.0, 10.0);
  MDT_COMPARE(aio->maximum(), 10.0, 12, -10.0, 10.0);
  QCOMPARE(aio->unit(), QString("[V]"));
  QCOMPARE(aio->addressRead(), 5);
  QVERIFY(aio->addressWrite() == aio->addressRead());

  /*
   * Check clear
   */
  m.clear();
  QCOMPARE(m.firstAiAddress() , 0);
  QCOMPARE(m.lastAiAddress() , 0);
  QCOMPARE(m.nextModuleFirstAiAddress() , 0);
  QCOMPARE(m.firstAoAddressRead() , 0);
  QCOMPARE(m.lastAoAddressRead() , 0);
  QCOMPARE(m.nextModuleFirstAoAddressRead() , 0);
  QCOMPARE(m.firstAoAddressWrite() , 0);
  QCOMPARE(m.lastAoAddressWrite() , 0);
  QCOMPARE(m.nextModuleFirstAoAddressWrite() , 0);
  ///QVERIFY(m.analogIos().isEmpty());
  QVERIFY(m.analogInputs().isEmpty());
  QVERIFY(m.analogOutputs().isEmpty());
  ///QVERIFY(m.digitalIos().isEmpty());
  QVERIFY(m.digitalInputs().isEmpty());
  QVERIFY(m.digitalOutputs().isEmpty());
  QVERIFY(m.type() == mdtDeviceModbusWagoModule::Unknown);
  QCOMPARE(m.partNumber() , 0);
  QVERIFY(m.partNumberText().isEmpty());

  /*
   * Analog OUT module
   *
   * Check with 750-550 witch has:
   *  - 2 analog OUT
   *  - Range: 0V to +10V
   *  - Unit: [V]
   *
   * We use a address range:
   *  - Read access: 6-7
   *  - Write access: 16-17
   */
  m.clear();
  QVERIFY(m.setupFromRegisterWord(550));
  m.updateAddresses(0, 6, 16, 0, 0, 0);
  QCOMPARE(m.firstAiAddress() , 0);
  QCOMPARE(m.lastAiAddress() , 0);
  QCOMPARE(m.nextModuleFirstAiAddress() , 0);
  QCOMPARE(m.firstAoAddressRead() , 6);
  QCOMPARE(m.lastAoAddressRead() , 7);
  QCOMPARE(m.nextModuleFirstAoAddressRead() , 8);
  QCOMPARE(m.firstAoAddressWrite() , 16);
  QCOMPARE(m.lastAoAddressWrite() , 17);
  QCOMPARE(m.nextModuleFirstAoAddressWrite() , 18);
  QCOMPARE(m.analogInputs().size(), 0);
  QCOMPARE(m.analogOutputs().size(), 2);
  /**
  m.setFirstAddress(6, 16);
  QCOMPARE(m.firstAddressRead() , 6);
  QCOMPARE(m.firstAddressWrite() , 16);
  QCOMPARE(m.lastAddressRead() , 7);
  QCOMPARE(m.lastAddressWrite() , 17);
  QCOMPARE(m.ioCount() , 2);
  QCOMPARE(m.analogIos().size(), 2);
  */
  ///QVERIFY(m.digitalIos().isEmpty());
  QVERIFY(m.digitalInputs().isEmpty());
  QVERIFY(m.digitalOutputs().isEmpty());
  QVERIFY(m.type() == mdtDeviceModbusWagoModule::AnalogOutputs);
  QCOMPARE(m.partNumber() , 550);
  QCOMPARE(m.partNumberText() , QString("550"));
  // Check internal I/O setup
  aio = m.analogOutputs().at(0);
  QVERIFY(aio != 0);
  MDT_COMPARE(aio->minimum(), 0.0, 12, 0.0, 10.0);
  MDT_COMPARE(aio->maximum(), 10.0, 12, 0.0, 10.0);
  QCOMPARE(aio->unit(), QString("[V]"));
  QCOMPARE(aio->addressRead(), 6);
  QCOMPARE(aio->addressWrite(), 16);
  aio = m.analogOutputs().at(1);
  QVERIFY(aio != 0);
  MDT_COMPARE(aio->minimum(), 0.0, 12, 0.0, 10.0);
  MDT_COMPARE(aio->maximum(), 10.0, 12, 0.0, 10.0);
  QCOMPARE(aio->unit(), QString("[V]"));
  QCOMPARE(aio->addressRead(), 7);
  QCOMPARE(aio->addressWrite(), 17);

  /*
   * Analog OUT module
   *
   * Check with 750-552 witch has:
   *  - 2 analog OUT
   *  - Range: 0 to +20mA
   *  - Unit: [mA]
   *
   * We use a address range:
   *  - Read access: 8-9
   *  - Write access: 18-19
   */
  m.clear();
  QVERIFY(m.setupFromRegisterWord(552));
  m.updateAddresses(0, 8, 18, 0, 0, 0);
  QCOMPARE(m.firstAiAddress() , 0);
  QCOMPARE(m.lastAiAddress() , 0);
  QCOMPARE(m.nextModuleFirstAiAddress() , 0);
  QCOMPARE(m.firstAoAddressRead() , 8);
  QCOMPARE(m.lastAoAddressRead() , 9);
  QCOMPARE(m.nextModuleFirstAoAddressRead() , 10);
  QCOMPARE(m.firstAoAddressWrite() , 18);
  QCOMPARE(m.lastAoAddressWrite() , 19);
  QCOMPARE(m.nextModuleFirstAoAddressWrite() , 20);
  QCOMPARE(m.analogInputs().size(), 0);
  QCOMPARE(m.analogOutputs().size(), 2);
  QVERIFY(m.digitalInputs().isEmpty());
  QVERIFY(m.digitalOutputs().isEmpty());
  QVERIFY(m.type() == mdtDeviceModbusWagoModule::AnalogOutputs);
  QCOMPARE(m.partNumber() , 552);
  QCOMPARE(m.partNumberText() , QString("552"));
  // Check internal I/O setup
  aio = m.analogOutputs().at(0);
  QVERIFY(aio != 0);
  MDT_COMPARE(aio->minimum(), 0.0, 12, 0.0, 20.0);
  MDT_COMPARE(aio->maximum(), 20.0, 12, 0.0, 20.0);
  QCOMPARE(aio->unit(), QString("[mA]"));
  QCOMPARE(aio->addressRead(), 8);
  QCOMPARE(aio->addressWrite(), 18);
  aio = m.analogOutputs().at(1);
  QVERIFY(aio != 0);
  MDT_COMPARE(aio->minimum(), 0.0, 12, 0.0, 20.0);
  MDT_COMPARE(aio->maximum(), 20.0, 12, 0.0, 20.0);
  QCOMPARE(aio->unit(), QString("[mA]"));
  QCOMPARE(aio->addressRead(), 9);
  QCOMPARE(aio->addressWrite(), 19);
  // Check some values
  aio->setValueInt(0, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 0.0, 12, 0.0, 20.0);
  aio->setValueInt(4096, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 2.5, 12, 0.0, 20.0);
  aio->setValueInt(8192, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 5.0, 12, 0.0, 20.0);
  aio->setValueInt(12288, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 7.5, 12, 0.0, 20.0);
  aio->setValueInt(16384, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 10.0, 12, 0.0, 20.0);
  aio->setValueInt(20480, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 12.5, 12, 0.0, 20.0);
  aio->setValueInt(24576, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 15.0, 12, 0.0, 20.0);
  aio->setValueInt(28672, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 17.5, 12, 0.0, 20.0);
  aio->setValueInt(32767, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 20.0, 12, 0.0, 20.0);

  /*
   * Analog OUT module
   *
   * Check with 750-556 witch has:
   *  - 2 analog OUT
   *  - Range: -10 to +10V
   *  - Unit: [V]
   *
   * We use a address range:
   *  - Read access: 8-9
   *  - Write access: 18-19
   */
  m.clear();
  QVERIFY(m.setupFromRegisterWord(556));
  m.updateAddresses(0, 8, 18, 0, 0, 0);
  QCOMPARE(m.firstAiAddress() , 0);
  QCOMPARE(m.lastAiAddress() , 0);
  QCOMPARE(m.nextModuleFirstAiAddress() , 0);
  QCOMPARE(m.firstAoAddressRead() , 8);
  QCOMPARE(m.lastAoAddressRead() , 9);
  QCOMPARE(m.nextModuleFirstAoAddressRead() , 10);
  QCOMPARE(m.firstAoAddressWrite() , 18);
  QCOMPARE(m.lastAoAddressWrite() , 19);
  QCOMPARE(m.nextModuleFirstAoAddressWrite() , 20);
  QCOMPARE(m.analogInputs().size(), 0);
  QCOMPARE(m.analogOutputs().size(), 2);
  QVERIFY(m.digitalInputs().isEmpty());
  QVERIFY(m.digitalOutputs().isEmpty());
  QVERIFY(m.type() == mdtDeviceModbusWagoModule::AnalogOutputs);
  QCOMPARE(m.partNumber() , 556);
  QCOMPARE(m.partNumberText() , QString("556"));
  // Check internal I/O setup
  aio = m.analogOutputs().at(0);
  QVERIFY(aio != 0);
  MDT_COMPARE(aio->minimum(), -10.0, 12, 0.0, 20.0);
  MDT_COMPARE(aio->maximum(), 10.0, 12, 0.0, 20.0);
  QCOMPARE(aio->unit(), QString("[V]"));
  QCOMPARE(aio->addressRead(), 8);
  QCOMPARE(aio->addressWrite(), 18);
  aio = m.analogOutputs().at(1);
  QVERIFY(aio != 0);
  MDT_COMPARE(aio->minimum(), -10.0, 12, 0.0, 20.0);
  MDT_COMPARE(aio->maximum(), 10.0, 12, 0.0, 20.0);
  QCOMPARE(aio->unit(), QString("[V]"));
  QCOMPARE(aio->addressRead(), 9);
  QCOMPARE(aio->addressWrite(), 19);
  // Check some values
  aio->setValueInt(0x8001, true, false);
  MDT_COMPARE(aio->value().valueDouble(), -10.0, 12, -10.0, 10.0);
  aio->setValueInt(0xA000, true, false);
  MDT_COMPARE(aio->value().valueDouble(), -7.5, 12, -10.0, 10.0);
  aio->setValueInt(0xC000, true, false);
  MDT_COMPARE(aio->value().valueDouble(), -5.0, 12, -10.0, 10.0);
  aio->setValueInt(0xE000, true, false);
  MDT_COMPARE(aio->value().valueDouble(), -2.5, 12, -10.0, 10.0);
  aio->setValueInt(0x000, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 0.0, 12, -10.0, 10.0);
  aio->setValueInt(0x2000, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 2.5, 12, -10.0, 10.0);
  aio->setValueInt(0x4000, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 5.0, 12, -10.0, 10.0);
  aio->setValueInt(0x6000, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 7.5, 12, -10.0, 10.0);
  aio->setValueInt(0x7FFF, true, false);
  MDT_COMPARE(aio->value().valueDouble(), 10.0, 12, -10.0, 10.0);

  /*
   * Digital IN module
   *
   * Check with a 4 digital input module.
   *
   * We use a address range:
   *  - Read access: 0-3
   *  - Write access: N/A
   */
  m.clear();
  QVERIFY(m.setupFromRegisterWord(0x8401));
  QVERIFY(m.type() == mdtDeviceModbusWagoModule::DigitalInputs);
  ///m.setFirstAddress(0);
  m.updateAddresses(0, 0, 0, 0, 0, 0);
  QCOMPARE(m.firstDiAddress(), 0);
  QCOMPARE(m.lastDiAddress(), 3);
  QCOMPARE(m.nextModuleFirstDiAddress(), 4);
  ///QCOMPARE(m.firstAddressRead() , 0);
  ///QCOMPARE(m.lastAddressRead() , 3);
  ///QCOMPARE(m.ioCount() , 4);
  QVERIFY(m.analogInputs().isEmpty());
  QVERIFY(m.analogOutputs().isEmpty());
  ///QCOMPARE(m.digitalIos().size(), 4);
  QCOMPARE(m.digitalInputs().size(), 4);
  QCOMPARE(m.digitalOutputs().size(), 0);
  // Check internal I/O setup
  ///dio = m.digitalIos().at(0);
  dio = m.digitalInputs().at(0);
  QVERIFY(dio != 0);
  QCOMPARE(dio->addressRead(), 0);
  QVERIFY(dio->addressWrite() == dio->addressRead());
  ///dio = m.digitalIos().at(1);
  dio = m.digitalInputs().at(1);
  QVERIFY(dio != 0);
  QCOMPARE(dio->addressRead(), 1);
  QVERIFY(dio->addressWrite() == dio->addressRead());
  ///dio = m.digitalIos().at(2);
  dio = m.digitalInputs().at(2);
  QVERIFY(dio != 0);
  QCOMPARE(dio->addressRead(), 2);
  QVERIFY(dio->addressWrite() == dio->addressRead());
  ///dio = m.digitalIos().at(3);
  dio = m.digitalInputs().at(3);
  QVERIFY(dio != 0);
  QCOMPARE(dio->addressRead(), 3);
  QVERIFY(dio->addressWrite() == dio->addressRead());

  /*
   * Digital OUT module
   *
   * Check with a 2 digital output module.
   *
   * We use a address range:
   *  - Read access: 4-5
   *  - Write access: 14-15
   */
  m.clear();
  QVERIFY(m.setupFromRegisterWord(0x8202));
  QVERIFY(m.type() == mdtDeviceModbusWagoModule::DigitalOutputs);
  ///m.setFirstAddress(4, 14);
  m.updateAddresses(0, 0, 0, 0, 4, 14);
  QCOMPARE(m.firstDoAddressRead(), 4);
  QCOMPARE(m.lastDoAddressRead(), 5);
  QCOMPARE(m.nextModuleFirstDoAddressRead(), 6);
  QCOMPARE(m.firstDoAddressWrite(), 14);
  QCOMPARE(m.lastDoAddressWrite(), 15);
  QCOMPARE(m.nextModuleFirstDoAddressWrite(), 16);
  ///QCOMPARE(m.firstAddressRead() , 4);
  ///QCOMPARE(m.firstAddressWrite() , 14);
  ///QCOMPARE(m.lastAddressRead() , 5);
  ///QCOMPARE(m.lastAddressWrite() , 15);
  ///QCOMPARE(m.ioCount() , 2);
  ///QVERIFY(m.analogIos().isEmpty());
  QVERIFY(m.analogInputs().isEmpty());
  QVERIFY(m.analogOutputs().isEmpty());
  ///QCOMPARE(m.digitalIos().size(), 2);
  QCOMPARE(m.digitalInputs().size(), 0);
  QCOMPARE(m.digitalOutputs().size(), 2);
  // Check internal I/O setup
  ///dio = m.digitalIos().at(0);
  dio = m.digitalOutputs().at(0);
  QVERIFY(dio != 0);
  QCOMPARE(dio->addressRead(), 4);
  QCOMPARE(dio->addressWrite(), 14);
  ///dio = m.digitalIos().at(1);
  dio = m.digitalOutputs().at(1);
  QVERIFY(dio != 0);
  QCOMPARE(dio->addressRead(), 5);
  QCOMPARE(dio->addressWrite(), 15);

  // free
  delete device;
}

void mdtDeviceTest::modbusWagoModuleRtdTest()
{
  mdtDeviceModbusWago *device = new mdtDeviceModbusWago;
  mdtDeviceModbusWagoModuleRtd *module = new mdtDeviceModbusWagoModuleRtd(true, device);
  mdtAnalogIo *ai;

  // Check initial values (Factory settings)
  QCOMPARE(module->firstAiAddress(), 0);
  QCOMPARE(module->firstAoAddressWrite(), 0);
  ///QCOMPARE(module->analogInputs().size(), 4);
  /**
  QVERIFY(module->analogInputs().at(0) != 0);
  QVERIFY(module->analogInputs().at(1) != 0);
  QVERIFY(module->analogInputs().at(2) != 0);
  QVERIFY(module->analogInputs().at(3) != 0);
  */
  QCOMPARE(module->analogOutputs().size(), 0);
  QCOMPARE(module->digitalInputs().size(), 0);
  QCOMPARE(module->digitalOutputs().size(), 4);
  QCOMPARE(module->sensorType(0), mdtDeviceModbusWagoModuleRtd::Pt100);
  QCOMPARE(module->sensorType(1), mdtDeviceModbusWagoModuleRtd::Pt100);
  QCOMPARE(module->sensorType(2), mdtDeviceModbusWagoModuleRtd::Pt100);
  QCOMPARE(module->sensorType(3), mdtDeviceModbusWagoModuleRtd::Pt100);
  /// \todo Check if true .....
  QCOMPARE(module->processValueRepresentation(0), mdtDeviceModbusWagoModuleRtd::C2);
  QCOMPARE(module->processValueRepresentation(1), mdtDeviceModbusWagoModuleRtd::C2);
  QCOMPARE(module->processValueRepresentation(2), mdtDeviceModbusWagoModuleRtd::C2);
  QCOMPARE(module->processValueRepresentation(3), mdtDeviceModbusWagoModuleRtd::C2);
  ///QVERIFY(module->psrrMode() == mdtDeviceModbusWagoModuleRtd::PsrrUnknown);
  
  /*
   * Simple set/get (on cached values) test
   */

  // PSRR mode
  module->setPsrrMode(mdtDeviceModbusWagoModuleRtd::PsrrUnknown);
  QVERIFY(module->psrrMode() == mdtDeviceModbusWagoModuleRtd::PsrrUnknown);
  module->setPsrrMode(mdtDeviceModbusWagoModuleRtd::Psrr50Hz);
  QVERIFY(module->psrrMode() == mdtDeviceModbusWagoModuleRtd::Psrr50Hz);
  module->setPsrrMode(mdtDeviceModbusWagoModuleRtd::Psrr60Hz);
  QVERIFY(module->psrrMode() == mdtDeviceModbusWagoModuleRtd::Psrr60Hz);
  module->setPsrrMode(mdtDeviceModbusWagoModuleRtd::Psrr5060Hz);
  QVERIFY(module->psrrMode() == mdtDeviceModbusWagoModuleRtd::Psrr5060Hz);
  // I/O count change - Must not affect I/O count
  module->setIoCount(2);
  QCOMPARE(module->analogInputs().size(), 0);
  module->setIoCount(4);
  QCOMPARE(module->analogInputs().size(), 0);
  // Check that setup works
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Pt100);
  module->setSensorType(1, mdtDeviceModbusWagoModuleRtd::Pt100);
  module->setSensorType(2, mdtDeviceModbusWagoModuleRtd::Pt100);
  module->setSensorType(3, mdtDeviceModbusWagoModuleRtd::Pt100);
  module->setProcessValueRepresentation(0, mdtDeviceModbusWagoModuleRtd::C2);
  module->setProcessValueRepresentation(1, mdtDeviceModbusWagoModuleRtd::C2);
  module->setProcessValueRepresentation(2, mdtDeviceModbusWagoModuleRtd::C2);
  module->setProcessValueRepresentation(3, mdtDeviceModbusWagoModuleRtd::C2);
  QVERIFY(module->setupFromRegisterWord(464));
  QCOMPARE(module->analogInputs().size(), 4);
  /*
   * Check addressing
   * We use a address range 2-5
   */
  module->updateAddresses(2, 0, 0, 0, 0, 0);
  QCOMPARE(module->firstAiAddress(), 2);
  QCOMPARE(module->lastAiAddress(), 5);
  QCOMPARE(module->nextModuleFirstAiAddress(), 6);
  QCOMPARE(module->nextModuleFirstAoAddressRead(), 0);
  QCOMPARE(module->nextModuleFirstAoAddressWrite(), 0);
  QCOMPARE(module->nextModuleFirstDiAddress(), 0);
  QCOMPARE(module->nextModuleFirstDoAddressRead(), 0);
  QCOMPARE(module->nextModuleFirstDoAddressWrite(), 0);
  // Sensor type
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Pt100);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::Pt100);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Ni100);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::Ni100);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Pt1000);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::Pt1000);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Pt500);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::Pt500);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Pt200);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::Pt200);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Ni1000);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::Ni1000);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Pt200);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::Pt200);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Ni120);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::Ni120);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Ni1000TK);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::Ni1000TK);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Potentiometer);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::Potentiometer);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::R10R5000);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::R10R5000);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::R10R1200);
  QVERIFY(module->sensorType(0) == mdtDeviceModbusWagoModuleRtd::R10R1200);

  /*
   * Setup module for 2 channel operation :
   *  - Channel 0: Pt100
   *  - Channel 1: Pt1000
   */
  module->clear();
  module->setIoCount(2);
  module->setSensorType(0, mdtDeviceModbusWagoModuleRtd::Pt100);
  module->setSensorType(1, mdtDeviceModbusWagoModuleRtd::Pt100);
  module->setProcessValueRepresentation(0, mdtDeviceModbusWagoModuleRtd::C2);
  module->setProcessValueRepresentation(1, mdtDeviceModbusWagoModuleRtd::C2);
  QVERIFY(module->setupFromRegisterWord(464));
  QCOMPARE(module->analogInputs().size(), 2);
  // Check channel 0 with Pt100
  ai = module->analogInputs().at(0);
  QVERIFY(ai != 0);
  ai->setValue(0xF830);
  MDT_COMPARE(ai->value().valueDouble(), -200.0, 16, -200.0, 850.0);
  ai->setValue(0x0000);
  MDT_COMPARE(ai->value().valueDouble(), 0.0, 16, -200.0, 850.0);
  ai->setValue(0x2134);
  MDT_COMPARE(ai->value().valueDouble(), 850.0, 16, -200.0, 850.0);
  /// \todo Check with other sensors

  // Free ...
  delete device;
  delete module;
}

void mdtDeviceTest::modbusWagoTest()
{
  mdtDeviceModbusWago d;
  mdtDeviceIosWidget *iosw;
  mdtDeviceWindow dw;
  int setGetWaitTime = 50;  // [ms]

  /*
   * Setup I/O's
   */

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(d.ios());
  /*
   * Setup device window
   */
  dw.setDevice(&d);
  dw.setIosWidget(iosw);
  dw.show();
  /*
   * We assume that we have 2 digital inputs, that are all OFF
   * Lets try to connect requesting harware node ID 0
   */
  if(!d.connectToDevice(mdtModbusHwNodeId(0,2,0), "Node0", 100)){
    QSKIP("No Wago 750 device found, or other error", SkipAll);
  }
  QVERIFY(d.currentState() == mdtDevice::State_t::Ready);
  QCOMPARE(d.alias(), QString("Node0"));
  QVERIFY(d.portManager()->isReady());
  qDebug() << "Analog outputs: " << d.analogOutputsCount();
  qDebug() << "Analog inputs: " << d.analogInputsCount();
  qDebug() << "Digital outputs: " << d.digitalOutputsCount();
  qDebug() << "Digital inputs: " << d.digitalInputsCount();
  QVERIFY(d.detectIos());
  iosw->setDeviceIos(d.ios());
  QVERIFY(d.ios() != 0);

  /*
   * Tests
   *
   * Note: we make set/get on outputs.
   *  Because a set or get can be done fast (2-30 ms),
   *  we let the PLC some time to update its cache (else it sometimes returns us a cached value)
   */

  /*
   * Analog inputs
   */
  if(d.ios()->analogInputsCount() > 1){
    // Get cached values - must be invalid because we never get them from device
    QVERIFY(!d.getAnalogInputValue(0, false).isValid());
    ///QVERIFY(!d.getAnalogInputValue(0, true, false).isValid());   // waitOnReply should be obseleted
    QVERIFY(!d.getAnalogInputValueAt(0, false).isValid());
    ///QVERIFY(!d.getAnalogInputValueAt(0, true, false).isValid()); // waitOnReply should be obseleted
    // Get values from device (and also check that cached getting returns valid value too)
    QVERIFY(d.getAnalogInputValue(0, true).isValid());
    QVERIFY(d.getAnalogInputValue(0, false).isValid());
    QVERIFY(d.getAnalogInputValueAt(0, true).isValid());
    QVERIFY(d.getAnalogInputValueAt(0, false).isValid());
    ///QVERIFY(!d.getAnalogInputValue(0, true, false).isValid());
    QVERIFY(d.getAnalogInputValue("AI1", true).isValid());
    ///QVERIFY(!d.getAnalogInputValue("AI1", true, false).isValid());
    QVERIFY(d.getAnalogInputValue(1, true).isValid());
    QVERIFY(d.getAnalogInputValue(1, false).isValid());
    QVERIFY(d.getAnalogInputValueAt(1, true).isValid());
    QVERIFY(d.getAnalogInputValueAt(1, false).isValid());
    QVERIFY(d.getAnalogInputValue("AI2", true).isValid());
    QVERIFY(d.getAnalogInputValue("AI2", false).isValid());
    // Check that getting all analog inputs works
    QVERIFY(d.getAnalogInputs());
    QVERIFY(d.getAnalogInputValue(0, false).isValid());
    QVERIFY(d.getAnalogInputValue(1, false).isValid());
  }else{
    qDebug() << "Not enough analog inputs available (expected 2) - Will not be tested";
  }

  // Analog outputs
  if(d.ios()->analogOutputsCount() > 1){
    // Get cached values - must be invalid because we never get them from device
//     QVERIFY(!d.getAnalogOutputValue(0x200, false).isValid());
//     QVERIFY(!d.getAnalogOutputValueAt(0, false).isValid());
    // Get values from device (and also check that cached getting returns valid value too)
    QVERIFY(d.getAnalogOutputValue(0x200, true).isValid());
    QVERIFY(d.getAnalogOutputValue(0x200, false).isValid());
    QVERIFY(d.getAnalogOutputValueAt(0, false).isValid());
    ///QVERIFY(!d.getAnalogOutputValue(0x200, true, false).isValid());
    QVERIFY(d.setAnalogOutputValue(0, 2.5, true));
    d.wait(setGetWaitTime);
    QVERIFY(d.getAnalogOutputValue(0x200, true).isValid());
    // Check that getting all analog outputs works
//    QVERIFY(d.getAnalogOutputs());
    QVERIFY(d.getAnalogOutputValue(0x200, false).isValid());
    MDT_COMPARE(d.getAnalogOutputValue(0x200, false).valueDouble(), 2.5, 12, -10.0, 10.0);
    MDT_COMPARE(d.getAnalogOutputValueAt(0, false).valueDouble(), 2.5, 12, -10.0, 10.0);
    QVERIFY(d.setAnalogOutputValue("AO1", 2.5, true));
    d.wait(setGetWaitTime);
    QVERIFY(d.getAnalogOutputValue("AO1", true).isValid());
    MDT_COMPARE(d.getAnalogOutputValue(0x200, true).valueDouble(), 2.5, 12, -10.0, 10.0);
    QVERIFY(d.getAnalogOutputValue(0x200, false).isValid());
    QVERIFY(d.setAnalogOutputValueAt(1, 3.5, true, true));
    d.wait(setGetWaitTime);
    QVERIFY(d.getAnalogOutputValue("AO1", true).isValid());
    MDT_COMPARE(d.getAnalogOutputValue(0x201, true).valueDouble(), 3.5, 12, -10.0, 10.0);
    // Grouped query
    QVERIFY(d.setAnalogOutputValue(0, 1.5, false));
    QVERIFY(d.setAnalogOutputValue(1, 2.5, false));
    QVERIFY(d.setAnalogOutputs());
    d.wait(setGetWaitTime);
    MDT_COMPARE(d.getAnalogOutputValue(0x200, true).valueDouble(), 1.5, 12, -10.0, 10.0);
    MDT_COMPARE(d.getAnalogOutputValue(0x201, true).valueDouble(), 2.5, 12, -10.0, 10.0);
    MDT_COMPARE(d.getAnalogOutputValue("AO1", true).valueDouble(), 1.5, 12, -10.0, 10.0);
    MDT_COMPARE(d.getAnalogOutputValue("AO2", true).valueDouble(), 2.5, 12, -10.0, 10.0);
  }else{
    qDebug() << "Not enough analog outputs available (expected 2) - Will not be tested";
  }

  // Digital inputs
  QVERIFY(!d.getDigitalInputValue(0, false).isValid());
  QVERIFY(!d.getDigitalInputValueAt(1, false).isValid());
  QVERIFY(d.getDigitalInputValue(0, true).isValid());
  QVERIFY(d.getDigitalInputValue(0, false).isValid());
  ///QVERIFY(!d.getDigitalInputValue(0, true, false).isValid());
  QVERIFY(d.getDigitalInputs());
  QVERIFY(d.getDigitalInputValue(0, false).isValid());

  // Digital outputs
  if(d.ios()->digitalOutputsCount() > 0){
    QVERIFY(!d.getDigitalOutputValue(0x200, false).isValid());
    QVERIFY(d.getDigitalOutputValue(0x200, true).isValid());
    QVERIFY(d.getDigitalOutputValue(0x200, false).isValid());
    ///QVERIFY(!d.getDigitalOutputValue(0x200, true, false).isValid());
    QVERIFY(d.getDigitalOutputs());
    QVERIFY(d.getDigitalOutputValue(0x200, false).isValid());
    QVERIFY(d.setDigitalOutputValue(0, true, true));
    d.wait(setGetWaitTime);
    QCOMPARE(d.getDigitalOutputValue(0x200, true).valueBool(), true);
    QVERIFY(d.setDigitalOutputValue(0, false, true));
    d.wait(setGetWaitTime);
    QCOMPARE(d.getDigitalOutputValue(0x200, true).valueBool(), false);
    QVERIFY(d.setDigitalOutputValue("DO1", true, true));
    d.wait(setGetWaitTime);
    QCOMPARE(d.getDigitalOutputValue("DO1", true).valueBool(), true);
    QCOMPARE(d.getDigitalOutputValue(0x200, true).valueBool(), true);
    QCOMPARE(d.getDigitalOutputValueAt(0, true).valueBool(), true);
    QVERIFY(d.setDigitalOutputValue("DO1", false, true));
    d.wait(setGetWaitTime);
    QCOMPARE(d.getDigitalOutputValue("DO1", true).valueBool(), false);
    QCOMPARE(d.getDigitalOutputValue(0x200, true).valueBool(), false);
    QVERIFY(d.setDigitalOutputValueAt(0, true, true));
    d.wait(setGetWaitTime);
    QCOMPARE(d.getDigitalOutputValue("DO1", true).valueBool(), true);
    // Grouped query
    QVERIFY(d.setDigitalOutputValue(0, true, false));
    QVERIFY(d.setDigitalOutputValue("DO2", true, false));
    QVERIFY(d.setDigitalOutputs());
    d.wait(setGetWaitTime);
    QCOMPARE(d.getDigitalOutputValue("DO1", true).valueBool(), true);
    QCOMPARE(d.getDigitalOutputValue(0x200, true).valueBool(), true);
    /*
     * Set a state to all digital outputs and check
     */
    // Set all DO ON
    QVERIFY(d.setDigitalOutputsValue(true, true));
    d.wait(setGetWaitTime);
    // Read back and check
    QVERIFY(d.getDigitalOutputs());
    for(int i = 0; i < d.digitalOutputsCount(); ++i){
      QCOMPARE(d.getDigitalOutputValue(i + 0x200, false).valueBool(), true);
    }
    // Set all DO OFF
    QVERIFY(d.setDigitalOutputsValue(false, true));
    d.wait(setGetWaitTime);
    // Read back and check
    QVERIFY(d.getDigitalOutputs());
    for(int i = 0; i < d.digitalOutputsCount(); ++i){
      QCOMPARE(d.getDigitalOutputValue(i + 0x200, false).valueBool(), false);
    }
  }else{
    qDebug() << "No digital outputs available - Will not be tested";
  }
  d.start(100);
  /*
  while(dw.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDeviceTest::modbusBeckhoffTest()
{
  QSKIP("Beckhoff is not supported for the moment", SkipAll);

  mdtDeviceModbus d;
  mdtDeviceIosWidget *iosw;
  mdtAnalogIo *ai;
  mdtAnalogIo *ao;
  mdtDigitalIo *di;
  mdtDigitalIo *dout;
  mdtDeviceWindow dw;

  /*
   * Setup I/O's
   */

  // Analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(0);
  ai->setLabelShort("AI1");
  ai->setUnit("[V]");
  ai->setDetails("Module type: KL3001");
  ai->setRange(-10.0, 10.0, 13, 3, true);
  d.addInput(ai);

  // Analog outputs
  ao = new mdtAnalogIo;
  ao->setAddress(0);
  ao->setLabelShort("AO1");
  ao->setUnit("[V]");
  ao->setDetails("Module type: KL4001");
  ///QVERIFY(ao->setRange(0.0, 10.0, 12, 3, false));
  ao->setRange(0.0, 10.0, 12, 3, false);
  ///QVERIFY(ao->setEncodeBitSettings(15, 0));
  d.addOutput(ao);

  // Digital inputs
  di = new mdtDigitalIo;
  di->setAddress(0);
  di->setLabelShort("DI1");
  di->setDetails("Module type: KL1002");
  d.addInput(di);
  di = new mdtDigitalIo;
  di->setAddress(1);
  di->setLabelShort("DI2");
  di->setDetails("Module type: KL1002");
  d.addInput(di);

  // Digital outputs
  dout = new mdtDigitalIo;
  dout->setAddress(0);
  dout->setLabelShort("DO1");
  dout->setDetails("Module type: KL2012");
  d.addOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(1);
  dout->setLabelShort("DO2");
  dout->setDetails("Module type: KL2012");
  d.addOutput(dout);

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(d.ios());
  ///iosw.show();

  // Setup device
  dw.setIosWidget(iosw);
  dw.show();

  /*
   * Tests
   */

  QVERIFY(d.getAnalogOutputs(500) >= 0);
  ///d.start(100);
  ///while(iosw.isVisible()){
  ///while(dw.isVisible()){
    QTest::qWait(1000);
  ///}
}

void mdtDeviceTest::scpiTest()
{
  mdtDeviceScpi d;
  mdtUsbtmcPortSetupDialog dialog;
  mdtUsbDeviceDescriptor deviceDescriptor;
  mdtScpiIdnResponse idnResponse;
  QByteArray data;

  /*
   * Default states
   */
  QCOMPARE(d.defaultTimeout(), 30000);
  // Try to find a USBTMC device
  dialog.scan();
  QTimer::singleShot(1000, &dialog, SLOT(accept()));
  dialog.exec();
  deviceDescriptor = dialog.selectedDevice();
  if(deviceDescriptor.isEmpty()){
    QSKIP("No USBTMC device attached to system.", SkipAll);
  }

  /*
   * Connect to device - Version that accepts idVendor, idProduct , SN and alias
   */
  QCOMPARE(d.alias(), QString(""));
  QCOMPARE((int)d.deviceAddress().usbIdVendor(), 0);
  QCOMPARE((int)d.deviceAddress().usbIdProduct(), 0);
  // Connect
  QVERIFY(d.connectToDevice(deviceDescriptor.idVendor(), deviceDescriptor.idProduct(), "", "DMM01"));
  // Check that attributes where updated
  QCOMPARE(d.deviceAddress().usbIdVendor(), deviceDescriptor.idVendor());
  QCOMPARE(d.deviceAddress().usbIdProduct(), deviceDescriptor.idProduct());
  QCOMPARE(d.alias(), QString("DMM01"));
  /*
   * Get device identification and try to reconnect with devices serial number
   */
  // Get device identification
  idnResponse = d.getDeviceIdentification();
  QVERIFY(!idnResponse.isNull());
  // Reconnect
  d.disconnectFromDevice();
  QVERIFY(d.connectToDevice(deviceDescriptor.idVendor(), deviceDescriptor.idProduct(), idnResponse.serial, "DMM02"));
  // Check that attributes where updated
  QCOMPARE(d.deviceAddress().usbIdVendor(), deviceDescriptor.idVendor());
  QCOMPARE(d.deviceAddress().usbIdProduct(), deviceDescriptor.idProduct());
  QCOMPARE(d.deviceAddress().usbDeviceSerialNumber(), idnResponse.serial);
  QCOMPARE(d.alias(), QString("DMM02"));

  // Check commands
  QVERIFY(d.sendCommand("*CLS\n"));
  QVERIFY(d.sendCommand("*RST\n"));
  ///QVERIFY(d.waitOperationComplete(5000, 100));
}

void mdtDeviceTest::U3606ATest()
{
  mdtDeviceU3606A d;
  mdtValueDouble x;
  mdtValueBool b;
  mdtScpiIdnResponse idnResponse;

  // Try to find a device and connect if ok
  if(!d.connectToDevice("U3606A-01")){
    qDebug() << "Connecting to U3606A device failed: " << d.lastError().text();
    QSKIP("No Agilent U3606A attached, or other error", SkipAll);
  }
  // Check that attributes where set
  QCOMPARE((int)d.deviceAddress().usbIdVendor(), 0x0957);
  QCOMPARE((int)d.deviceAddress().usbIdProduct(), 0x4d18);
  QCOMPARE(d.alias(), QString("U3606A-01"));
  /*
   * Get device identification and try to reconnect with devices serial number
   */
  QVERIFY(d.sendCommand("*CLS\n"));
  d.wait(1);
  // Get device identification
  idnResponse = d.getDeviceIdentification();
  QVERIFY(!idnResponse.isNull());
  QCOMPARE(idnResponse.manufacturer, QString("Agilent Technologies"));
  QCOMPARE(idnResponse.model, QString("U3606A"));
  // Reconnect
  d.disconnectFromDevice();
  QVERIFY(d.connectToDevice(idnResponse.serial, "U3606A-02"));
  // Check that attributes where set
  QCOMPARE((int)d.deviceAddress().usbIdVendor(), 0x0957);
  QCOMPARE((int)d.deviceAddress().usbIdProduct(), 0x4d18);
  QCOMPARE(d.deviceAddress().usbDeviceSerialNumber(), idnResponse.serial);
  QCOMPARE(d.alias(), QString("U3606A-02"));
  // Reset device
  QVERIFY(d.reset(true, true));
  // Check generic command
//   QVERIFY(d.sendCommand("*CLS\n"));
//   d.wait(1);
//   QVERIFY(d.sendCommand("*RST\n"));
//   d.wait(1);
  QCOMPARE(d.getDeviceError().systemNumber(), 0);
  d.wait(1);
  QVERIFY(d.sendQuery("*IDN?\n").left(27) == "Agilent Technologies,U3606A");
  d.wait(1);

  // Set and get device setup
  QVERIFY(d.sendCommand("CONF:VOLT 10,0.001\n"));
  d.wait(1);
  QVERIFY(d.getMeasureConfiguration() == mdtDeviceU3606A::MeasureType_t::VoltageDc);
  QVERIFY(d.setupVoltageDcMeasure(mdtDeviceU3606A::Range_t::Auto, mdtDeviceU3606A::Resolution_t::Min));
  QVERIFY(d.getMeasureConfiguration() == mdtDeviceU3606A::MeasureType_t::VoltageDc);
  x = d.getMeasureValue();
  qDebug() << "----------> Voltage: " << x;
  QVERIFY(!x.isNull());
  /*
   * Check source functions
   */
  // Reset device
  QVERIFY(d.reset(true, true));
  // Setup source as constant current source
  QVERIFY(d.setupSourceCurrent(0.5, 5.0, 7.0));
  // Check that source output is OFF
  b = d.getSourceOutputState();
  QVERIFY(!b.isNull());
  QVERIFY(b == false);
  d.wait(1);
  // Check source current
  x = d.getSourceCurrent();
  QVERIFY(!x.isNull());
  QVERIFY(x == 0.5);
  d.wait(1);
  // Check source voltage limit
  x = d.getSourceVoltageLimit();
  QVERIFY(!x.isNull());
  QVERIFY(x == 5.0);
  d.wait(1);
  // Check OVP
  x = d.getSourceVoltageProtection();
  QVERIFY(!x.isNull());
  QVERIFY(x == 7.0);
  d.wait(1);
  // Setup source as constant voltage source
  QVERIFY(d.setupSourceVoltage(5.0, 2.0, 2.5));
  // Check that source output is OFF
  b = d.getSourceOutputState();
  QVERIFY(!b.isNull());
  QVERIFY(b == false);
  d.wait(1);
  // Check source voltage
  x = d.getSourceVoltage();
  QVERIFY(!x.isNull());
  QVERIFY(x == 5.0);
  d.wait(1);
  // Check source current limit
  x = d.getSourceCurrentLimit();
  qDebug() << "--------> x: " << x;
  QVERIFY(!x.isNull());
  QVERIFY(x == 2.0);
  d.wait(1);
  // Check OCP
  x = d.getSourceCurrentProtection();
  QVERIFY(!x.isNull());
  QVERIFY(x == 2.5);
  d.wait(1);


  // Enable bits in Status Byte Register
  QVERIFY(d.sendCommand("*SRE 255\n"));
  ///qDebug() << "*SRE?: " << d.sendQuery("*SRE?\n");
  
  // Enable bits in Standard Events Register
  QVERIFY(d.sendCommand("*ESE 255\n"));
  ///qDebug() << "*ESE?: " << d.sendQuery("*ESE?\n");
  
  // Enable bits in Standard Operation Regsiter
  QVERIFY(d.sendCommand(":STAT:OPER:ENAB 65535\n"));
  ///qDebug() << "STATus:OPERation:ENABle?: " << d.sendQuery("STATus:OPERation:ENABle?\n");

  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");

  ///qDebug() << "*STB?: " << d.sendQuery("*STB?\n");
  
  ///qDebug() << "*IDN?: " << d.sendQuery("*IDN?\n");
  
  ///QTest::qWait(3000);

  
  ///d.sendQuery("*OPC?\n");
  ///QTest::qWait(2000);
  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  // Check generic query
  
  
  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");

  // Get value
  ///QVERIFY(d.getAnalogInputValue(0, 32000).isValid());
//   QVERIFY(d.getAnalogInputValue(0, true, true).isValid());
//   QVERIFY(d.getAnalogInputValueAt(0, true, true).isValid());
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  qDebug() << "*** OPC: " << d.sendQuery("*OPC?\n");
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  ///QVERIFY(d.getAnalogInputValue(0, 32000).type() == QVariant::Double);
//   QVERIFY(d.getAnalogInputValue(0, true, true).isValid());
  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
//   d.checkDeviceError();
  
//   d.start(100);
  QTest::qWait(1000);
  /**
  while(dw.isVisible()){
    QTest::qWait(500);
  }
  */
}

void mdtDeviceTest::DSO1000ATest()
{
  mdtDeviceDSO1000A d;
  ///mdtAnalogIo *ai;
  ///mdtDeviceIos ios;
  ///mdtDeviceIosWidget *iosw;
  mdtDeviceWindow dw;
  mdtFrameCodecScpi codec;
  QByteArray data;

  // Try to find a device and connect if ok
  /// \todo Update mdtDeviceDSO1000A like mdtDeviceU3606A for connectToDevice() !
  if(!d.connectToDevice()){
    qDebug() << "Connecting to DSO1000A device failed: " << d.lastError().text();
    QSKIP("No Agilent DSO1000A attached, or other error", SkipAll);
  }

  // Check generic command
  QVERIFY(d.sendCommand("*RST\n"));
  d.wait(1);
  QVERIFY(d.sendCommand("*CLS\n"));
  d.wait(1);
  QCOMPARE(d.getDeviceError().systemNumber(), 0);
  d.wait(1);

  QVERIFY(d.sendCommand(":CHANnel1:PROBe 20X\n"));
  QVERIFY(d.sendCommand(":ACQuire:TYPE PEAKdetect\n"));
  ///QVERIFY(d.sendCommand(":ACQuire:AVERages 8\n") >= 0);
  QVERIFY(d.sendCommand(":TRIGger:MODE EDGE\n"));
  QVERIFY(d.sendCommand(":TRIGger:EDGE:SOURce CHANnel1\n"));
  QVERIFY(d.sendCommand(":TRIGger:EDGE:SWEep AUTO\n"));
  QVERIFY(d.sendCommand(":TIMebase:MAIN:SCALe 50e-3\n"));
  QVERIFY(d.sendCommand(":START\n") >= 0);
  ///d.portManager()->wait(10000);
  QTest::qWait(10000);
  QVERIFY(d.sendCommand(":STOP\n"));
  QVERIFY(d.sendCommand(":WAVeform:SOURce CHANnel1\n"));
  QVERIFY(d.sendCommand(":WAVeform:FORMat BYTE\n"));
  
  qDebug() << "TEST: getting data ...";
  
  QVERIFY(d.waitOperationComplete(5000, 1000));
  ///d.checkDeviceError();  /// \todo re-enable once correted
  
  
  double y_inc, y_val, y_ref, y_origin;
  
  qDebug() << "TEST: getting preamble ...";
  
  data = d.sendQuery(":WAVeform:PREamble?\n");
  qDebug() << codec.decodeValues(data, ",");
  qDebug() << codec.values();
  QVERIFY(codec.values().size() > 9);
  y_inc = codec.values().at(7).toDouble();
  qDebug() << "Y inc: " << y_inc;
  y_origin = codec.values().at(8).toDouble();
  qDebug() << "Y O: " << y_origin;
  y_ref = codec.values().at(9).toDouble();
  qDebug() << "Y ref: " << y_ref;
  
  
  data = d.sendQuery(":WAVeform:DATA?\n");
  qDebug() << "Data len: " << data.size();
  
  QVERIFY(codec.decodeIEEEblock(data, mdtFrameCodecScpi::BYTE));
  for(int i=0; i<codec.values().size(); i++){
    ///qDebug() << "data[" << i << "]: " << codec.values().at(i) << " , flt: " << codec.values().at(i).toDouble()*y_inc;
    y_val = codec.values().at(i).toDouble();
    ///qDebug() << "data[" << i << "]: " << ((y_ref - y_val) * y_inc) - y_origin;
  }
  ///qDebug() << "Data: " << codec.values();

}


int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtDeviceTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}

