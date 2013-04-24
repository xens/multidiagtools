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
#include "mdtDeviceTest.h"
#include "mdtApplication.h"
#include "mdtDeviceIos.h"
#include "mdtDeviceIosWidget.h"
#include "mdtDeviceModbus.h"
#include "mdtModbusTcpPortManager.h"
#include "mdtDeviceModbusWago.h"
#include "mdtDeviceScpi.h"
#include "mdtDeviceU3606A.h"
#include "mdtDeviceDSO1000A.h"
#include "mdtDeviceWindow.h"
#include "mdtPortInfo.h"

#include "mdtFrameCodecScpi.h"

#include <QString>
#include <QVariant>
#include <QList>

#include <QDebug>

void mdtDeviceTest::deviceBaseTest()
{
  mdtDevice dev;

  // Initial states
  QVERIFY(dev.state() == mdtDevice::Disconnected);

  dev.start(100);
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

  // Add analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(12);
  ios.addAnalogInput(ai);
  ai = new mdtAnalogIo;
  ai->setAddress(10);
  ai->setLabelShort("AI10");
  ios.addAnalogInput(ai);

  // Add analog outputs
  ao = new mdtAnalogIo;
  ao->setAddressRead(15);
  ao->setAddressWrite(115);
  ios.addAnalogOutput(ao);
  ao = new mdtAnalogIo;
  ao->setAddressRead(8);
  ao->setAddressWrite(108);
  ao->setLabelShort("AO8");
  ios.addAnalogOutput(ao);

  // Add digital inputs
  di = new mdtDigitalIo;
  di->setAddress(17);
  ios.addDigitalInput(di);

  // Add digital inputs
  dout = new mdtDigitalIo;
  dout->setAddress(20);
  ios.addDigitalOutput(dout);

  // Check analog inputs
  QCOMPARE(ios.analogInputsCount(), 2);
  QCOMPARE(ios.analogInputsFirstAddress(), 10);
  QVERIFY(ios.analogInputAt(10) != 0);
  QCOMPARE(ios.analogInputAt(10)->address() , 10);
  QVERIFY(ios.analogInputWithLabelShort("AI10") != 0);
  QCOMPARE(ios.analogInputWithLabelShort("AI10")->address(), 10);
  QCOMPARE(ios.analogInputWithLabelShort("AI10")->labelShort(), QString("AI10"));
  QVERIFY(ios.analogInputAt(11) == 0);
  QVERIFY(ios.analogInputAt(12) != 0);
  QCOMPARE(ios.analogInputAt(12)->address() , 12);
  ios.analogInputAt(12)->setLabelShort("Temp. M12");
  QVERIFY(ios.analogInputWithLabelShort("") == 0);
  QVERIFY(ios.analogInputWithLabelShort("Temp. M12") != 0);
  QCOMPARE(ios.analogInputWithLabelShort("Temp. M12")->address(), 12);
  QCOMPARE(ios.analogInputWithLabelShort("Temp. M12")->labelShort(), QString("Temp. M12"));
  aIoList = ios.analogInputs();
  QCOMPARE(aIoList.size(), 2);
  values.clear();
  values << 1.0 << 12.5;
  ios.updateAnalogInputValues(values);
  QCOMPARE(ios.analogInputAt(10)->value() , 1.0);
  QCOMPARE(ios.analogInputAt(12)->value() , 12.5);
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
  // Check digital inputs
  QVERIFY(ios.digitalInputAt(11) == 0);
  QVERIFY(ios.digitalInputAt(17) != 0);
  QVERIFY(ios.digitalInputAt(17)->address() == 17);
  ios.digitalInputAt(17)->setLabelShort("DI17");
  QVERIFY(ios.digitalInputWithLabelShort("") == 0);
  QVERIFY(ios.digitalInputWithLabelShort("DI17") != 0);
  QCOMPARE(ios.digitalInputWithLabelShort("DI17")->address(), 17);
  QCOMPARE(ios.digitalInputWithLabelShort("DI17")->labelShort(), QString("DI17"));
  dIoList = ios.digitalInputs();
  QCOMPARE(dIoList.size(), 1);

  // Check digital outputs
  QVERIFY(ios.digitalOutputAt(19) == 0);
  QVERIFY(ios.digitalOutputAt(20) != 0);
  QVERIFY(ios.digitalOutputAt(20)->address() == 20);
  ios.digitalOutputAt(20)->setLabelShort("K20");
  QVERIFY(ios.digitalOutputWithLabelShort("") == 0);
  QVERIFY(ios.digitalOutputWithLabelShort("K20") != 0);
  QCOMPARE(ios.digitalOutputWithLabelShort("K20")->address(), 20);
  QCOMPARE(ios.digitalOutputWithLabelShort("K20")->labelShort(), QString("K20"));
  dIoList = ios.digitalOutputs();
  QCOMPARE(dIoList.size(), 1);

}

void mdtDeviceTest::deviceIosWidgetTest()
{
  mdtDeviceIosWidget w;
  mdtDeviceIos ios;
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
  QVERIFY(ai->setRange(0.0, 10.0, 16));
  ///ios.addAnalogInput(ai);

  ai = new mdtAnalogIo;
  ai->setAddress(13);
  ai->setLabelShort("wX_BatUBa");
  ai->setLabel("rBatSpannung");
  ai->setDetails("Batteriespannung\nSchema: 06E");
  QVERIFY(ai->setRange(0.0, 50.0, 16));
  ios.addAnalogInput(ai);

  // Add analog outputs
  ao = new mdtAnalogIo;
  ao->setAddress(15);
  ao->setLabelShort("Vout");
  ao->setLabel("Output voltage for AG speed");
  ao->setDetails("Output voltage for AG speed setpoint\nSee schema 16F");
  QVERIFY(ao->setRange(0.0, 10.0, 16));
  ios.addAnalogOutput(ao);

  // Add digital inputs
  di = new mdtDigitalIo;
  di->setAddress(17);
  di->setLabelShort("Test");
  di->setLabel("Test mode");
  di->setDetails("Test mode enabled.\nIf ON, it's possible to set some outputs from tool\nSee schema 25G");
  ios.addDigitalInput(di);

  di = new mdtDigitalIo;
  di->setAddress(18);
  di->setLabelShort("xM_HSEin");
  di->setLabel(tr("xHochspannung"));
  di->setDetails("Hoschspannung vorhanden\nSchema: 06E");
  ios.addDigitalInput(di);

  di = new mdtDigitalIo;
  di->setAddress(19);
  di->setLabelShort("xM_MgBr");
  di->setLabel(tr("xMgBremse"));
  di->setDetails("Magnetische Bremse\nSchema: 06E");
  ios.addDigitalInput(di);

  // Add digital outputs
  dout = new mdtDigitalIo;
  dout->setAddress(20);
  dout->setLabelShort("xB_BelEin");
  dout->setLabel(tr("xBeleuchtungEin"));
  dout->setDetails(tr("Drehschalter Beleuchtung Ein") + "\n" + tr("Schema: 10A"));
  ios.addDigitalOutput(dout);

  dout = new mdtDigitalIo;
  dout->setAddress(21);
  dout->setLabelShort("xB_BatMinU");
  dout->setLabel(tr("xBatMinSpg"));
  dout->setDetails(tr("Batterie Minimalspannung") + "\n" + tr("Schema: 06C"));
  ios.addDigitalOutput(dout);

  // Setup widget
  ///w.setMaxColumns(3);
  w.setDeviceIos(&ios);
  w.show();

  /**
  while(w.isVisible()){
    QTest::qWait(1000);
  }
  */
  // Second setup (must clear existing widget and redo layout)
  w.setDeviceIos(&ios);
  w.show();
  /**
  while(w.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtDeviceTest::modbusTest()
{
  mdtDeviceModbus d;
  mdtModbusTcpPortManager *m = d.modbusTcpPortManager();
  QList<mdtPortInfo*> portInfoList;
  int hwNodeId = 3;
  mdtDeviceIos ios;
  mdtDeviceIosWidget *iosw;
  mdtDigitalIo *di;
  mdtDeviceWindow dw;

  QVERIFY(m != 0);

  // Setup some digital inputs (they are not device dependent)
  di = new mdtDigitalIo;
  di->setAddress(0);
  di->setLabelShort("DI1");
  di->setDetails("Some details about digital input 1");
  ios.addDigitalInput(di);  
  di = new mdtDigitalIo;
  di->setAddress(1);
  di->setLabelShort("DI2");
  di->setDetails("Some details about digital input 2");
  ios.addDigitalInput(di);  

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(&ios);

  // Setup device
  d.setIos(&ios, true);
  dw.setDevice(&d);
  dw.setIosWidget(iosw);
  dw.show();

  // Scan looking in chache file first
  portInfoList = m->scan(m->readScanResult());
  // Try to connect ...
  if(d.connectToDevice(portInfoList, hwNodeId, 4) != mdtAbstractPort::NoError){
    // scan network an try again
    qDeleteAll(portInfoList);
    portInfoList.clear();
    qDebug() << "Scanning network ...";
    portInfoList = m->scan(QNetworkInterface::allInterfaces(), 502, 100);
    if(d.connectToDevice(portInfoList, hwNodeId, 4) != mdtAbstractPort::NoError){
      QSKIP("Modbus device with requested harware node ID not found", SkipAll);
    }
    // Ok found, save scan result
    QVERIFY(m->saveScanResult(portInfoList));
    // We no lobger need portInfoList
    qDeleteAll(portInfoList);
    portInfoList.clear();
  }

  // Make some queries
  QVERIFY(!d.getDigitalInputState(0, 0).isValid());
  QVERIFY(d.getDigitalOutputs(500) >= 0);
  d.start(100);
  while(dw.isVisible()){
    QTest::qWait(1000);
  }
}

void mdtDeviceTest::modbusWagoTest()
{
  mdtDeviceModbusWago d;
  mdtDeviceIos ios;
  mdtDeviceIosWidget *iosw;
  mdtDeviceWindow dw;

  /*
   * Setup I/O's
   */

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(&ios);

  // Setup device
  d.setIos(&ios, false);
  ///d.setAnalogOutputAddressOffset(0x0200);
  ///d.setDigitalOutputAddressOffset(0x0200);
  dw.setDevice(&d);
  dw.setIosWidget(iosw);
  dw.show();

  if(d.connectToDevice(mdtDeviceInfo()) != mdtAbstractPort::NoError){;
    QSKIP("No Wago 750 device found, or other error", SkipAll);
  }
  QVERIFY(d.portManager()->isRunning());
  qDebug() << "Analog outputs: " << d.analogOutputsCount();
  qDebug() << "Analog inputs: " << d.analogInputsCount();
  qDebug() << "Digital outputs: " << d.digitalOutputsCount();
  qDebug() << "Digital inputs: " << d.digitalInputsCount();
  QVERIFY(d.detectIos(&ios));
  d.setIos(&ios, true);
  iosw->setDeviceIos(&ios);

  /*
   * Tests
   */

  // Analog inputs
  QVERIFY(d.getAnalogInputValue(0, true, true, true).isValid());
  QVERIFY(!d.getAnalogInputValue(0, true, true, false).isValid());
  QVERIFY(d.getAnalogInputValue(1, true, true, true).isValid());
  QVERIFY(d.getAnalogInputValue(1, true, false, false).isValid());
  QVERIFY(d.getAnalogInputs(500) >= 0);

  // Analog outputs
  QVERIFY(d.setAnalogOutputValue(0, 2.5, 500) >= 0);
  QVERIFY(d.getAnalogOutputValue(0, 500, true).isValid());
  QVERIFY(d.getAnalogOutputs(500) >= 0);
  QVERIFY(d.setAnalogOutputValue(0, 2.5, 500) >= 0);
  QVERIFY(d.getAnalogOutputValue(0, 500, true).isValid());
  QVERIFY(qAbs(d.getAnalogOutputValue(0, 500, true).toDouble()-2.5) < (10.0/4050.0));
  QVERIFY(!d.getAnalogOutputValue(0, 0, true).isValid());
  // Grouped query
  QVERIFY(d.setAnalogOutputValue(0, 1.5, -1) == 0);
  QVERIFY(d.setAnalogOutputValue(1, 2.5, -1) == 0);
  QVERIFY(d.setAnalogOutputs(500) >= 0);
  QVERIFY(qAbs(d.getAnalogOutputValue(0, 500, true).toDouble()-1.5) < (10.0/4050.0));
  QVERIFY(qAbs(d.getAnalogOutputValue(1, 500, true).toDouble()-2.5) < (10.0/4050.0));

  // Digital inputs
  QVERIFY(!d.getDigitalInputState(0, 0).isValid());
  ///QVERIFY(d.getDigitalInputState(0, 500).isValid());
  ///QTest::qWait(500);
  qDebug() << "State: " << d.getDigitalInputState(0, 500);

  // Digital outputs
  QVERIFY(!d.getDigitalOutputState(0, 0).isValid());
  QVERIFY(d.getDigitalOutputState(0, 500).isValid());
  // Grouped query
  QVERIFY(d.setDigitalOutputState(0, true, false, false) == 0);
  QVERIFY(d.setDigitalOutputState("DO2", true, false, false) == 0);
  QVERIFY(d.setDigitalOutputState(8, true, false, false) == 0);
  QVERIFY(d.setDigitalOutputs(500) >= 0);

  ///QVERIFY(d.getDigitalInputs(500) >= 0);
  QVERIFY(d.getAnalogOutputs(500) >= 0);
  QVERIFY(d.getDigitalOutputs(500) >= 0);
  d.start(100);
  while(dw.isVisible()){
    QTest::qWait(1000);
  }
}

void mdtDeviceTest::modbusBeckhoffTest()
{
  QSKIP("Beckhoff is not supported for the moment", SkipAll);

  mdtDeviceModbus d;
  mdtDeviceIos ios;
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
  ios.addAnalogInput(ai);

  // Analog outputs
  ao = new mdtAnalogIo;
  ao->setAddress(0);
  ao->setLabelShort("AO1");
  ao->setUnit("[V]");
  ao->setDetails("Module type: KL4001");
  QVERIFY(ao->setRange(0.0, 10.0, 12, 3, false));
  QVERIFY(ao->setEncodeBitSettings(15, 0));
  ios.addAnalogOutput(ao);

  // Digital inputs
  di = new mdtDigitalIo;
  di->setAddress(0);
  di->setLabelShort("DI1");
  di->setDetails("Module type: KL1002");
  ios.addDigitalInput(di);
  di = new mdtDigitalIo;
  di->setAddress(1);
  di->setLabelShort("DI2");
  di->setDetails("Module type: KL1002");
  ios.addDigitalInput(di);

  // Digital outputs
  dout = new mdtDigitalIo;
  dout->setAddress(0);
  dout->setLabelShort("DO1");
  dout->setDetails("Module type: KL2012");
  ios.addDigitalOutput(dout);
  dout = new mdtDigitalIo;
  dout->setAddress(1);
  dout->setLabelShort("DO2");
  dout->setDetails("Module type: KL2012");
  ios.addDigitalOutput(dout);

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(&ios);
  ///iosw.show();

  // Setup device
  d.setIos(&ios, true);
  d.setAnalogOutputAddressOffset(0x0800);
  d.setDigitalOutputAddressOffset(0);
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
  QList<mdtPortInfo*> portInfoList;
  mdtPortInfo *portInfo;
  mdtDeviceInfo *devInfo;
  mdtFrameCodecScpi codec;
  QByteArray data;

  // Try to find a device and connect if ok
  portInfoList = d.portManager()->scan();
  if(portInfoList.size() < 1){
    QSKIP("No port found with SCPI device found, or other error", SkipAll);
  }
  portInfo = portInfoList.at(0);
  if(portInfo->deviceInfoList().size() < 1){
    QSKIP("No SCPI device found, or other error", SkipAll);
  }
  devInfo = portInfo->deviceInfoList().at(0);
  if(d.connectToDevice(*devInfo) != mdtAbstractPort::NoError){
    QSKIP("No SCPI device attached, or other error", SkipAll);
  }

  // Check commands
  QVERIFY(d.sendCommand("*RST\n") >= 0);
  QVERIFY(d.waitOperationComplete(5000, 100));
  QVERIFY(d.sendCommand("*CLS\n") >= 0);
  // Check query
  QVERIFY(!d.sendQuery("*IDN?\n").isEmpty());
}

void mdtDeviceTest::U3606ATest()
{
  mdtDeviceU3606A d;
  mdtDeviceInfo devInfo;
  mdtAnalogIo *ai;
  mdtDeviceIos ios;
  mdtDeviceIosWidget *iosw;
  mdtDeviceWindow dw;

  // Try to find a device and connect if ok
  if(d.connectToDevice(devInfo) != mdtAbstractPort::NoError){
    QSKIP("No Agilent U3606A attached, or other error", SkipAll);
  }

  /*
   * Setup I/O's
   */

  // Analog inputs
  ai = new mdtAnalogIo;
  ai->setAddress(0);
  ai->setLabelShort("Voltage");
  ai->setUnit("[V]");
  //ai->setDetails("Module type: KL3001");
  ai->setRange(0.0, 1000.0, 32);
  ios.addAnalogInput(ai);

  /*
   * Setup devie
   */

  // Setup I/O's widget
  iosw = new mdtDeviceIosWidget;
  iosw->setDeviceIos(&ios);

  // Setup device
  d.setIos(&ios, true);
  dw.setDevice(&d);
  dw.setIosWidget(iosw);
  dw.statusWidget()->setStateBusyText("Query running ...");
  dw.statusWidget()->setStateBusyColor(mdtLed::LED_COLOR_GREEN);
  QVERIFY(d.portManager()->writeThread() != 0);
  QVERIFY(d.portManager()->readThread() != 0);
  dw.statusWidget()->enableTxRxLeds(d.portManager()->writeThread(), d.portManager()->readThread());
  dw.show();

  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");

  // Check generic command
  QVERIFY(d.sendCommand("*CLS\n") >= 0);
  QVERIFY(d.sendCommand("*RST\n") >= 0);
  
  // Enable bits in Status Byte Register
  QVERIFY(d.sendCommand("*SRE 255\n") >= 0);
  ///qDebug() << "*SRE?: " << d.sendQuery("*SRE?\n");
  
  // Enable bits in Standard Events Register
  QVERIFY(d.sendCommand("*ESE 255\n") >= 0);
  ///qDebug() << "*ESE?: " << d.sendQuery("*ESE?\n");
  
  // Enable bits in Standard Operation Regsiter
  QVERIFY(d.sendCommand(":STAT:OPER:ENAB 65535\n") >= 0);
  ///qDebug() << "STATus:OPERation:ENABle?: " << d.sendQuery("STATus:OPERation:ENABle?\n");

  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");

  ///qDebug() << "*STB?: " << d.sendQuery("*STB?\n");
  
  ///qDebug() << "*IDN?: " << d.sendQuery("*IDN?\n");
  
  ///QTest::qWait(3000);

  
  ///d.sendQuery("*OPC?\n");
  ///QTest::qWait(2000);
  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  // Check generic query
  QVERIFY(d.sendQuery("*IDN?\n").left(27) == "Agilent Technologies,U3606A");
  
  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");

  // Get value
  ///QVERIFY(d.getAnalogInputValue(0, 32000).isValid());
  QVERIFY(d.getAnalogInputValue(0, true, true, true).isValid());
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  qDebug() << "*** OPC: " << d.sendQuery("*OPC?\n");
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  ///QVERIFY(d.getAnalogInputValue(0, 32000).type() == QVariant::Double);
  QVERIFY(d.getAnalogInputValue(0, true, true, true).isValid());
  ///qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");
  d.checkDeviceError();
  
  d.start(500);
  QTest::qWait(5000);
  
  while(dw.isVisible()){
    QTest::qWait(500);
  }
  
}

void mdtDeviceTest::DSO1000ATest()
{
  mdtDeviceDSO1000A d;
  mdtDeviceInfo devInfo;
  ///mdtAnalogIo *ai;
  ///mdtDeviceIos ios;
  ///mdtDeviceIosWidget *iosw;
  mdtDeviceWindow dw;
  mdtFrameCodecScpi codec;
  QByteArray data;

  // Try to find a device and connect if ok
  if(d.connectToDevice(devInfo) != mdtAbstractPort::NoError){
    QSKIP("No Agilent DSO1000A attached, or other error", SkipAll);
  }

  // Check generic command
  QVERIFY(d.sendCommand("*CLS\n") >= 0);
  QVERIFY(d.sendCommand("*RST\n") >= 0);
  QVERIFY(d.sendQuery("*IDN?\n").left(26) == "Agilent Technologies,DSO10");
  qDebug() << "*** Err: " << d.sendQuery("SYST:ERR?\n");

  QVERIFY(d.sendCommand(":CHANnel1:PROBe 20X\n") >= 0);
  QVERIFY(d.sendCommand(":ACQuire:TYPE PEAKdetect\n") >= 0);
  ///QVERIFY(d.sendCommand(":ACQuire:AVERages 8\n") >= 0);
  QVERIFY(d.sendCommand(":TRIGger:MODE EDGE\n") >= 0);
  QVERIFY(d.sendCommand(":TRIGger:EDGE:SOURce CHANnel1\n") >= 0);
  QVERIFY(d.sendCommand(":TRIGger:EDGE:SWEep AUTO\n") >= 0);
  QVERIFY(d.sendCommand(":TIMebase:MAIN:SCALe 50e-3\n") >= 0);
  QVERIFY(d.sendCommand(":START\n") >= 0);
  d.portManager()->wait(10000);
  QVERIFY(d.sendCommand(":STOP\n") >= 0);
  QVERIFY(d.sendCommand(":WAVeform:SOURce CHANnel1\n") >= 0);
  QVERIFY(d.sendCommand(":WAVeform:FORMat BYTE\n") >= 0);
  
  qDebug() << "TEST: getting data ...";
  
  QVERIFY(d.waitOperationComplete(5000, 1000));
  d.checkDeviceError();
  
  
  double y_inc, y_val, y_ref, y_origin;
  
  qDebug() << "TEST: getting preamble ...";
  
  data = d.sendQuery(":WAVeform:PREamble?\n");
  qDebug() << codec.decodeValues(data, ",");
  qDebug() << codec.values();
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
    qDebug() << "data[" << i << "]: " << ((y_ref - y_val) * y_inc) - y_origin;
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

