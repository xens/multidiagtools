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
#include "mdtIoWidgetTest.h"
#include "mdtAnalogIo.h"
#include "mdtDigitalIo.h"
#include "mdtAnalogInWidget.h"
#include "mdtAnalogOutWidget.h"
#include "mdtDigitalInWidget.h"
#include "mdtDigitalOutWidget.h"

#include <QTest>
#include <QString>
#include <QVariant>
#include <QPushButton>
#include "mdtApplication.h"

#include <QDebug>

/*
 * mdtTestSlider class
 */
mdtTestSlider::mdtTestSlider(QWidget *parent)
 : QwtSlider(parent)
{
}

void mdtTestSlider::setValue(const mdtValue &value)
{
  QwtSlider::setValue(value.valueDouble());
}

/*
 * Tests
 */

void mdtIoWidgetTest::analogInWidgetTest()
{
  mdtAnalogIo ai;
  mdtAnalogInWidget w;

  w.setIo(&ai);
  w.show();

  // Initial state
  QVERIFY(!ai.hasValidData());
  QCOMPARE(ai.value().valueDouble(), 0.0);
  QCOMPARE(ai.value().valueInt(), 0);

  // 0...10V range with 8 bits resolution
  ai.setLabel("Input voltage");
  ai.setLabelShort("Vin");
  ai.setUnit("V");
  ai.setRange(0, 10, 8);
  MDT_COMPARE(ai.value().valueDouble(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ai.value().valueInt(), 0);
  ai.setValueInt(0, true, false);
  MDT_COMPARE(ai.value().valueDouble(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ai.value().valueInt(), 0);
  ai.setValueInt(127, true, false);
  MDT_COMPARE(ai.value().valueDouble(), 5.0, 8, 0.0, 10.0);
  QCOMPARE(ai.value().valueInt(), 127);
  ai.setValueInt(255, true, false);
  MDT_COMPARE(ai.value().valueDouble(), 10.0, 8, 0.0, 10.0);
  QCOMPARE(ai.value().valueInt(), 255);

  // 4...20mA range with 8 bits resolution
  ai.setLabel("Input current");
  ai.setLabelShort("Iin");
  ai.setDetails("Input current that is used for M1 setpoint.\nShema: 10E");
  ai.setAddressRead(1246);
  ai.setAddressWrite(564);
  ai.setUnit("mA");
  ai.setRange(4, 20, 8);
  MDT_COMPARE(ai.value().valueDouble(), 4.0, 8, 4.0, 20.0);
  QCOMPARE(ai.value().valueInt(), 0);
  ai.setValueInt(0, true, false);
  QCOMPARE(ai.value().valueDouble(), 4.0);
  QCOMPARE(ai.value().valueInt(), 0);
  ai.setValueInt(127, true, false);
  MDT_COMPARE(ai.value().valueDouble(), 12.0, 8, 4.0, 20.0);
  QCOMPARE(ai.value().valueInt(), 127);
  ai.setValueInt(255, true, false);
  MDT_COMPARE(ai.value().valueDouble(), 20.0, 8, 4.0, 20.0);
  QCOMPARE(ai.value().valueInt(), 255);

  /*
  while(w.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtIoWidgetTest::analogOutWidgetTest()
{
  mdtAnalogIo ai, ao;
  mdtAnalogOutWidget wAo;
  mdtAnalogInWidget wAi;

  // Setup
  wAi.setIo(&ai);
  wAo.setIo(&ao);
  QObject::connect(&ao, SIGNAL(valueChanged(const mdtValue&)), &ai, SLOT(setValue(const mdtValue&)));

  wAi.show();
  wAo.show();

  // Initial state
  QVERIFY(!ai.hasValidData());
  QCOMPARE(ai.value().valueDouble(), 0.0);
  QCOMPARE(ai.value().valueInt(), 0);
  QVERIFY(!ao.hasValidData());
  QCOMPARE(ao.value().valueDouble(), 0.0);
  QCOMPARE(ao.value().valueInt(), 0);

  // 0...10V range with 8 bits resolution
  ao.setLabel("Output voltage of final stage");
  ao.setLabelShort("Vout");
  ao.setRange(0, 10, 8);
  ai.setLabel("Input voltage of final stage");
  ai.setLabelShort("Vin");
  ai.setUnit("V");
  ai.setRange(0, 10, 8);
  MDT_COMPARE(ao.value().valueDouble(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ao.value().valueInt(), 0);
  MDT_COMPARE(ai.value().valueDouble(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ai.value().valueInt(), 0);
  ao.setValueInt(0, true, true);
  MDT_COMPARE(ao.value().valueDouble(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ao.value().valueInt(), 0);
  MDT_COMPARE(ai.value().valueDouble(), 0.0, 8, 0.0, 10.0);
  QCOMPARE(ai.value().valueInt(), 0);
  ao.setValueInt(127, true, true);
  MDT_COMPARE(ao.value().valueDouble(), 5.0, 8, 0.0, 10.0);
  MDT_COMPARE(ao.value().valueInt(), 127, 8, 0, 255);
  MDT_COMPARE(ai.value().valueDouble(), 5.0, 8, 0.0, 10.0);
  MDT_COMPARE(ai.value().valueInt(), 127, 8, 0, 255);
  ao.setValueInt(255, true, true);
  MDT_COMPARE(ao.value().valueDouble(), 10.0, 8, 0.0, 10.0);
  MDT_COMPARE(ao.value().valueInt(), 255, 8, 0, 255);
  MDT_COMPARE(ai.value().valueDouble(), 10.0, 8, 0.0, 10.0);
  MDT_COMPARE(ai.value().valueInt(), 255, 8, 0, 255);

  // 4...20mA range with 8 bits resolution
  ao.setLabel("Output current of final stage");
  ao.setLabelShort("Iout");
  ao.setRange(4, 20, 8);
  ai.setRange(4, 20, 8);
  ai.setLabelShort("Iin");
  ai.setUnit("mA");
  MDT_COMPARE(ao.value().valueDouble(), 4.0, 8, 4.0, 20.0);
  MDT_COMPARE(ao.value().valueInt(), 0, 8, 0, 255);
  MDT_COMPARE(ai.value().valueDouble(), 4.0, 8, 4.0, 20.0);
  MDT_COMPARE(ai.value().valueInt(), 0, 8, 0, 255);
  ao.setValueInt(0, true, true);
  MDT_COMPARE(ao.value().valueDouble(), 4.0, 8, 4.0, 20.0);
  MDT_COMPARE(ao.value().valueInt(), 0, 8, 0, 255);
  MDT_COMPARE(ai.value().valueDouble(), 4.0, 8, 4.0, 20.0);
  MDT_COMPARE(ai.value().valueInt(), 0, 8, 0, 255);
  ao.setValueInt(127, true, true);
  MDT_COMPARE(ao.value().valueDouble(), 12.0, 8, 4.0, 20.0);
  MDT_COMPARE(ao.value().valueInt(), 127, 8, 0, 255);
  MDT_COMPARE(ai.value().valueDouble(), 12.0, 8, 4.0, 20.0);
  MDT_COMPARE(ai.value().valueInt(), 127, 8, 0, 255);
  ao.setValueInt(255, true, true);
  MDT_COMPARE(ao.value().valueDouble(), 20.0, 8, 4.0, 20.0);
  MDT_COMPARE(ao.value().valueInt(), 255, 8, 0, 255);
  MDT_COMPARE(ai.value().valueDouble(), 20.0, 8, 4.0, 20.0);
  MDT_COMPARE(ai.value().valueInt(), 255, 8, 0, 255);
}

void mdtIoWidgetTest::analogOutWidgetRecursifTest()
{
  // GUI (program)
  mdtAnalogIo ao;
  mdtTestSlider sl(0);
  // Simulate the physical device (test that signal valueChanged() is emitted)
  mdtAnalogIo plcAo;

  // Setup: range: 0-10, n = 12 bits
  ao.setLabelShort("AO1");
  ao.setRange(0.0, 10.0, 12);
  plcAo.setLabelShort("PLC");
  plcAo.setRange(0.0, 10.0, 12);
  sl.setScale(0.0, 10.0);
  QObject::connect(&sl, SIGNAL(valueChanged(double)), &ao, SLOT(setValueFromUi(double)));
  QObject::connect(&ao, SIGNAL(valueChangedForUi(const mdtValue&)), &sl, SLOT(setValue(const mdtValue&)));
  QObject::connect(&ao, SIGNAL(valueChanged(const mdtValue&)), &plcAo, SLOT(setValue(const mdtValue&)));
  sl.show();

  // Initial states
  MDT_COMPARE(ao.value().valueDouble(), 0.0, 12, 0.0, 10.0);
  MDT_COMPARE(plcAo.value().valueDouble(), 0.0, 12, 0.0, 10.0);

  // User change the value
  sl.setValue(1.0);
  MDT_COMPARE(ao.value().valueDouble(), 1.0, 12, 0.0, 10.0);
  MDT_COMPARE(plcAo.value().valueDouble(), 1.0, 12, 0.0, 10.0);
  // PLC (device) confirm the same value
  ao.setValue(1.0, false);
  MDT_COMPARE(ao.value().valueDouble(), 1.0, 12, 0.0, 10.0);
  MDT_COMPARE(plcAo.value().valueDouble(), 1.0, 12, 0.0, 10.0);

  // User change the value
  sl.setValue(2.0);
  MDT_COMPARE(ao.value().valueDouble(), 2.0, 12, 0.0, 10.0);
  MDT_COMPARE(plcAo.value().valueDouble(), 2.0, 12, 0.0, 10.0);
  // PLC (device) confirm a value that differs
  ao.setValue(1.5, false);
  MDT_COMPARE(ao.value().valueDouble(), 1.5, 12, 0.0, 10.0);
  // Check that PLC not receives the confirmation as new value
  MDT_COMPARE(plcAo.value().valueDouble(), 2.0, 12, 0.0, 10.0);

  // User change the value
  sl.setValue(3.5);
  MDT_COMPARE(ao.value().valueDouble(), 3.5, 12, 0.0, 10.0);
  MDT_COMPARE(plcAo.value().valueDouble(), 3.5, 12, 0.0, 10.0);
  // PLC (device) confirm the same value
  ao.setValue(3.5, false);
  MDT_COMPARE(ao.value().valueDouble(), 3.5, 12, 0.0, 10.0);
  MDT_COMPARE(plcAo.value().valueDouble(), 3.5, 12, 0.0, 10.0);

  // User change the value
  sl.setValue(4.0);
  MDT_COMPARE(ao.value().valueDouble(), 4.0, 12, 0.0, 10.0);
  MDT_COMPARE(plcAo.value().valueDouble(), 4.0, 12, 0.0, 10.0);
  // PLC (device) confirm the same value
  ao.setValue(4.0, false);
  MDT_COMPARE(ao.value().valueDouble(), 4.0, 12, 0.0, 10.0);
  MDT_COMPARE(plcAo.value().valueDouble(), 4.0, 12, 0.0, 10.0);

  // We change the value from application
  ao.setValue(2.6, true);
  // PLC (device) confirm the same value
  ao.setValue(2.6, false);
  MDT_COMPARE(ao.value().valueDouble(), 2.6, 12, 0.0, 10.0);
  MDT_COMPARE(plcAo.value().valueDouble(), 2.6, 12, 0.0, 10.0);

  // We change the value from application
  ao.setValue(1.3, true);
  // PLC (device) confirm another value
  ao.setValue(1.0, false);
  MDT_COMPARE(ao.value().valueDouble(), 1.0, 12, 0.0, 10.0);
  MDT_COMPARE(plcAo.value().valueDouble(), 1.3, 12, 0.0, 10.0);

  /*
  while(sl.isVisible()){
    QTest::qWait(100);
  }
  */
}

void mdtIoWidgetTest::digitalInWidgetTest()
{
  mdtDigitalIo di;
  mdtDigitalInWidget wDi;

  wDi.show();

  // Setup
  wDi.setIo(&di);
  di.setLabelShort("I2");
  di.setLabel("Input 2 (Drive ON)");
  di.setDetails("Used for drive M on");

  // Initial state
  QCOMPARE(di.value().valueBool(), false);
  QVERIFY(!di.hasValidData());

  // Toggle ...
  di.setValue(true);
  QCOMPARE(di.value().valueBool(), true);
  di.setValue(false);
  QCOMPARE(di.value().valueBool(), false);
}

void mdtIoWidgetTest::digitalOutWidgetTest()
{
  mdtDigitalIo di;
  mdtDigitalIo dout;
  mdtDigitalOutWidget wDo;
  mdtDigitalInWidget wDi;

  // Setup
  wDi.setIo(&di);
  di.setLabelShort("I3");
  di.setLabel("Input 3 (AC1 Ok)");
  di.setDetails("Confirm that AC1 (3P var out for comp.) is Ok");
  dout.setLabelShort("O3");
  dout.setLabel("Output 3 (AC1 Ok)");
  dout.setDetails("Send confirmation that AC1 (3P var out for comp.) is Ok");
  wDo.setIo(&dout);
  QObject::connect(&dout, SIGNAL(valueChanged(const mdtValue&)), &di, SLOT(setValue(const mdtValue&)));

  wDo.show();
  wDi.show();

  // Initial state
  QCOMPARE(di.value().valueBool(), false);
  QCOMPARE(dout.value().valueBool(), false);
  QVERIFY(!di.hasValidData());
  QVERIFY(!dout.hasValidData());
  wDo.show();
  wDi.show();
  // Toggle ...
  dout.setValue(true, true);
  QCOMPARE(dout.value().valueBool(), true);
  QCOMPARE(di.value().valueBool(), true);
  dout.setValue(false, true);
  QCOMPARE(dout.value().valueBool(), false);
  QCOMPARE(di.value().valueBool(), false);

  /*
  while(wDo.isVisible()){
    QTest::qWait(1000);
  }
  */
}

void mdtIoWidgetTest::digitalOutWidgetRecursifTest()
{
  // GUI (program)
  mdtDigitalIo dout;
  mdtDigitalOutWidget doW;
  doW.setIo(&dout);

  doW.show();

  // Initial state
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "??");

  // A request was sent to PLC, and confirm arrives
  dout.setValue(false);
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "OFF");

  // User pressed button
  doW.internalPushButton()->setChecked(true);
  QVERIFY(doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "ON");
  // Request was sent to PLC and confirmation arrives with same state (On)
  dout.setValue(true);
  QVERIFY(doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "ON");

  // User pressed button
  doW.internalPushButton()->setChecked(false);
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "OFF");
  // Request was sent to PLC and confirmation arrives with same state (Off)
  dout.setValue(false);
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "OFF");

  // User pressed button
  doW.internalPushButton()->setChecked(true);
  dout.setEnabled(false);
  QVERIFY(doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "ON");
  // Request was sent to PLC and confirmation arrives with other state (Off)
  dout.setValue(false);
  dout.setEnabled(true);
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "OFF");

  // User pressed button
  doW.internalPushButton()->setChecked(true);
  QVERIFY(doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "ON");
  // Request was sent to PLC but a error occured
  dout.setValue(mdtValue());
  QVERIFY(!doW.internalPushButton()->isChecked());
  QVERIFY(doW.internalPushButton()->text() == "??");
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtIoWidgetTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
