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
#include "mdtFrameCodecTest.h"
#include "mdtFrameCodec.h"
#include "mdtFrameCodecAscii.h"
#include "mdtFrameCodecScpi.h"
#include "mdtFrameCodecScpiU3606A.h"
#include "mdtFrameCodecModbus.h"
#include "mdtFrameCodecK8055.h"
#include "mdtApplication.h"
#include <QByteArray>
#include <QVariant>
#include <QDate>
#include <QList>

#include <QDebug>

void mdtFrameCodecTest::asciiDecodeTest()
{
  QVariant value;
  mdtFrameCodecAscii codec;

  // Empty data
  value = codec.convertData("");
  QVERIFY(!value.isValid());

  // Integer value
  value = codec.convertData("1234");
  QVERIFY(value.isValid());
  QVERIFY(value.type() == QVariant::Int);
  QCOMPARE(value.toInt(), 1234);
  value = codec.convertData("+1234");
  QVERIFY(value.isValid());
  QVERIFY(value.type() == QVariant::Int);
  QCOMPARE(value.toInt(), 1234);
  value = codec.convertData("-1234");
  QVERIFY(value.isValid());
  QVERIFY(value.type() == QVariant::Int);
  QCOMPARE(value.toInt(), -1234);

  // Integer value
  value = codec.convertData("1234.5");
  QVERIFY(value.isValid());
  QVERIFY(value.type() == QVariant::Double);
  QCOMPARE(value.toDouble(), 1234.5);

  // Date value
  value = codec.convertData("2012.11.30");
  QVERIFY(value.isValid());
  QVERIFY(value.type() == QVariant::Date);
  QCOMPARE(value.toDate(), QDate(2012, 11, 30));

  // String value
  value = codec.convertData("ABCD");
  QVERIFY(value.isValid());
  QVERIFY(value.type() == QVariant::String);
  QCOMPARE(value.toString(), QString("ABCD"));

}

void mdtFrameCodecTest::scpiDecodeTest()
{
  mdtFrameCodecScpi codec;
  QByteArray data;
  ///QVariant value;
  mdtValue value;
  QByteArray eofSeq = "\n";

  codec.setEofSeq(eofSeq);

  // Empty data frame
  data = "";
  QVERIFY(!codec.decodeValues(data));
  QVERIFY(codec.values().size() == 0);
  data = "\n";
  QVERIFY(!codec.decodeValues(data));
  QVERIFY(codec.values().size() == 0);

  // Decode integer value
  data = "1234";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  ///QVERIFY(codec.values().at(0).type() == QVariant::Int);
  ///QCOMPARE(codec.values().at(0).toInt(), 1234);
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  QCOMPARE(codec.values().at(0).value<mdtValue>().valueInt(),  1234);

  data = "1234\n";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  ///QVERIFY(codec.values().at(0).type() == QVariant::Int);
  ///QCOMPARE(codec.values().at(0).toInt(), 1234);
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  QCOMPARE(codec.values().at(0).value<mdtValue>().valueInt(),  1234);

  // Decode integer value
  data = ":1234";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  ///QVERIFY(codec.values().at(0).type() == QVariant::Int);
  ///QCOMPARE(codec.values().at(0).toInt(), 1234);
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  QCOMPARE(codec.values().at(0).value<mdtValue>().valueInt(),  1234);

  /*
   * Check with some known data
   */

  data = "DBM";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  QVERIFY(codec.values().at(0).type() == QVariant::String);
  QCOMPARE(codec.values().at(0).toString(), QString("DBM"));

  data = "1";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  ///QVERIFY(codec.values().at(0).type() == QVariant::Int);
  ///QCOMPARE(codec.values().at(0).toInt(), 1);
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  QCOMPARE(codec.values().at(0).value<mdtValue>().valueInt(),  1);

  data = " +1.007850E+01";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  ///QVERIFY(codec.values().at(0).type() == QVariant::Double);
  ///QCOMPARE(codec.values().at(0).toDouble(),  1.007850E+01);
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  QCOMPARE(codec.values().at(0).value<mdtValue>().valueDouble(),  1.007850E+01);

  data = "-1.000000E+01";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  ///QVERIFY(codec.values().at(0).type() == QVariant::Double);
  ///QCOMPARE(codec.values().at(0).toDouble(),  -1.000000E+01);
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  QCOMPARE(codec.values().at(0).value<mdtValue>().valueDouble(),  -1.000000E+01);

  data = "-1.000000E+01\n";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  ///QVERIFY(codec.values().at(0).type() == QVariant::Double);
  ///QCOMPARE(codec.values().at(0).toDouble(),  -1.000000E+01);
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  QCOMPARE(codec.values().at(0).value<mdtValue>().valueDouble(),  -1.000000E+01);

  data = "+0";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  ///QVERIFY(codec.values().at(0).type() == QVariant::Int);
  ///QCOMPARE(codec.values().at(0).toInt(),  0);
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  QCOMPARE(codec.values().at(0).value<mdtValue>().valueInt(),  0);

  data = "0";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  ///QVERIFY(codec.values().at(0).type() == QVariant::Int);
  ///QCOMPARE(codec.values().at(0).toInt(),  0);
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  QCOMPARE(codec.values().at(0).value<mdtValue>().valueInt(),  0);

  data = "+739";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  QVERIFY(codec.values().at(0).isValid());
  ///QVERIFY(codec.values().at(0).type() == QVariant::Int);
  ///QCOMPARE(codec.values().at(0).toInt(),  739);
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  QCOMPARE(codec.values().at(0).value<mdtValue>().valueInt(),  739);

  data = "CAL: 27 Nov 2009";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 2);
  QVERIFY(codec.values().at(0).isValid());
  QVERIFY(codec.values().at(0).type() == QVariant::String);
  QCOMPARE(codec.values().at(0).toString(),  QString("CAL"));
  QVERIFY(codec.values().at(1).isValid());
  QVERIFY(codec.values().at(1).type() == QVariant::String);
  QCOMPARE(codec.values().at(1).toString(),  QString(" 27 Nov 2009"));

  // Decoding answer of *IDN? query
  data = "Agilent Technologies,U3606A,MY00000000,02.00-02.00-02.01\n";
  QVERIFY(codec.decodeIdn(data));
  QVERIFY(codec.values().size() == 4);
  QVERIFY(codec.values().at(0) == "Agilent Technologies");
  QVERIFY(codec.values().at(1) == "U3606A");
  QVERIFY(codec.values().at(2) == "MY00000000");
  QVERIFY(codec.values().at(3) == "02.00-02.00-02.01");

  // Decoding error (SCPI-99, chap. 21.8)
  data = " -133,\"Undefined header\"";
  QVERIFY(codec.decodeError(data));
  QVERIFY(codec.values().size() == 2);
  QVERIFY(codec.values().at(0).isValid());
  QVERIFY(codec.values().at(0) == -133);
  QVERIFY(codec.values().at(1) == "Undefined header");
  data = "-131,\"Invalid suffix;FREQuency:CENT 2.0E+5 dbuV\"";
  QVERIFY(codec.decodeError(data));
  QVERIFY(codec.values().size() == 3);
  QVERIFY(codec.values().at(0).isValid());
  QVERIFY(codec.values().at(0) == -131);
  QVERIFY(codec.values().at(1) == "Invalid suffix");
  QVERIFY(codec.values().at(2) == "FREQuency:CENT 2.0E+5 dbuV");

  // Float OL (SCPI-99, chap. 7.2.1)
  data = "9.9e37";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  ///QVERIFY(!codec.values().at(0).isValid());
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(codec.values().at(0).value<mdtValue>().isPlusOl());
  data = "9.999e37";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  ///QVERIFY(!codec.values().at(0).isValid());
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(codec.values().at(0).value<mdtValue>().isPlusOl());

  // Float -OL (SCPI-99, chap. 7.2.1)
  data = "-9.9e37";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  ///QVERIFY(!codec.values().at(0).isValid());
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());
  data = "-9.998e37";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  ///QVERIFY(!codec.values().at(0).isValid());
  QVERIFY(codec.values().at(0).value<mdtValue>().isValid());
  QVERIFY(codec.values().at(0).value<mdtValue>().isMinusOl());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isPlusOl());

  // Float NAN (SCPI-99, chap. 7.2.1)
  data = "9.91e37";
  QVERIFY(codec.decodeValues(data));
  QVERIFY(codec.values().size() == 1);
  ///QVERIFY(!codec.values().at(0).isValid());
  QVERIFY(!codec.values().at(0).value<mdtValue>().isValid());

  /// \todo re-enable commenetd value tests !!
  
  // Decode single value
  data = "+3.197000E-06\n";
  value = codec.decodeSingleValueDouble(data);
  QVERIFY(value.isValid());
  ///QCOMPARE(value.toDouble(), 3.197000E-06);
  QCOMPARE(value.valueDouble(), 3.197000E-06);
  data = "+2.930320E-01\n";
  value = codec.decodeSingleValueDouble(data);
  QVERIFY(value.isValid());
  QCOMPARE(value.valueDouble(), 0.2930320);

  // Decode single value: OL
  data = "+9.900000E+37\n";
  value = codec.decodeSingleValueDouble(data);
  ///QVERIFY(!value.isValid());
  ///QCOMPARE(value.toDouble(), 0.0);
  ///QCOMPARE(value.valueDouble(), 0.0);
  QVERIFY(value.isValid());
  QVERIFY(!value.isMinusOl());
  QVERIFY(value.isPlusOl());

  // Decode single value: -OL
  data = "-9.900000E+37\n";
  value = codec.decodeSingleValueDouble(data);
  ///QVERIFY(!value.isValid());
  ///QCOMPARE(value.toDouble(), 0.0);
  ///QCOMPARE(value.valueDouble(), 0.0);
  QVERIFY(value.isValid());
  QVERIFY(value.isMinusOl());
  QVERIFY(!value.isPlusOl());

  // Decode single value: invalid format
  data = "1.90000A0E+6\n";
  value = codec.decodeSingleValueDouble(data);
  QVERIFY(!value.isValid());
  ///QCOMPARE(value.toDouble(), 0.0);
  QCOMPARE(value.valueDouble(), 0.0);

  // Read IEEE block of data in ASCII format
  data = "#8000000311.000e-001,8.000e-002,2.000e001\n";
  QVERIFY(codec.decodeIEEEblock(data, mdtFrameCodecScpi::ASCII));
  QCOMPARE(codec.values().size(), 3);
  QCOMPARE(codec.values().at(0), QVariant(0.1));
  QCOMPARE(codec.values().at(1), QVariant(8.0e-2));
  QCOMPARE(codec.values().at(2), QVariant(20.0));

  // CONFigure? response
  data = "VOLT +1.000000E+01,+1.000000E-06\n";
  QVERIFY(codec.decodeFunctionParameters(data));
  QCOMPARE(codec.values().size(), 3);
  QVERIFY(codec.values().at(0).type() == QVariant::String);
  QCOMPARE(codec.values().at(0), QVariant("VOLT"));
  ///QCOMPARE(codec.values().at(1), QVariant(1.0e1));
  ///QCOMPARE(codec.values().at(2), QVariant(1.0e-6));
  QCOMPARE(codec.values().at(1).value<mdtValue>().valueDouble(), 1.0e1);
  QCOMPARE(codec.values().at(2).value<mdtValue>().valueDouble(), 1.0e-6);
  data = " VOLT +1.100000E+03,-1.200000E-05\n";
  QVERIFY(codec.decodeFunctionParameters(data));
  QCOMPARE(codec.values().size(), 3);
  QVERIFY(codec.values().at(0).type() == QVariant::String);
  QCOMPARE(codec.values().at(0), QVariant("VOLT"));
  ///QCOMPARE(codec.values().at(1), QVariant(1.1e3));
  ///QCOMPARE(codec.values().at(2), QVariant(-1.2e-5));
  QCOMPARE(codec.values().at(1).value<mdtValue>().valueDouble(), 1.1e3);
  QCOMPARE(codec.values().at(2).value<mdtValue>().valueDouble(), -1.2e-5);
  data = " CURR:AC +1.000000E-01,+1.000000E-05\n ";
  QVERIFY(codec.decodeFunctionParameters(data));
  QCOMPARE(codec.values().size(), 3);
  QVERIFY(codec.values().at(0).type() == QVariant::String);
  QCOMPARE(codec.values().at(0), QVariant("CURR:AC"));
  ///QCOMPARE(codec.values().at(1), QVariant(1e-1));
  ///QCOMPARE(codec.values().at(2), QVariant(1e-5));
  QCOMPARE(codec.values().at(1).value<mdtValue>().valueDouble(), 1e-1);
  QCOMPARE(codec.values().at(2).value<mdtValue>().valueDouble(), 1e-5);
  // FUNC? response
  data = "VOLT:AC\n";
  QVERIFY(codec.decodeFunctionParameters(data));
  QCOMPARE(codec.values().size(), 1);
  QVERIFY(codec.values().at(0).type() == QVariant::String);
  QCOMPARE(codec.values().at(0), QVariant("VOLT:AC"));
  data = " VOLT:DC ";
  QVERIFY(codec.decodeFunctionParameters(data));
  QCOMPARE(codec.values().size(), 1);
  QVERIFY(codec.values().at(0).type() == QVariant::String);
  QCOMPARE(codec.values().at(0), QVariant("VOLT:DC"));

  // SYST:VERS? response
  data = "1999.0";
  QVERIFY(codec.decodeScpiVersion(data));
  QCOMPARE(codec.values().size(), 2);
  QVERIFY(codec.values().at(0).type() == QVariant::Int);
  QCOMPARE(codec.values().at(0), QVariant(1999));
  QCOMPARE(codec.values().at(1), QVariant(0));
  data = "1990.1\n";
  QVERIFY(codec.decodeScpiVersion(data));
  QCOMPARE(codec.values().size(), 2);
  QVERIFY(codec.values().at(0).type() == QVariant::Int);
  QCOMPARE(codec.values().at(0), QVariant(1990));
  QCOMPARE(codec.values().at(1), QVariant(1));
}

void mdtFrameCodecTest::scpiU3606ATest()
{
  mdtFrameCodecScpiU3606A f;
  QByteArray data;
  QByteArray eofSeq = "\n";
  QVariant value;

  // Ihnitial states
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_UNKNOW);
  QVERIFY(!f.measureRange().isValid());
  QVERIFY(!f.resolution().isValid());

  ///QVERIFY(f.values().size() == 1);
  ///QCOMPARE(f.values().at(0).toDouble(), 3.197000E-06);


  // Decoding CONF? query answer
  data = "VOLT +1.000000E+01,+1.000000E-06\n";
  QVERIFY(f.decodeConfigure(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_VOLTAGE_DC);
  QVERIFY(f.measureRange().isValid());
  ///QCOMPARE(f.measureRange().toDouble(), 10.0);
  QCOMPARE(f.measureRange().valueDouble(), 10.0);
  QVERIFY(f.resolution().isValid());
  ///QCOMPARE(f.resolution().toDouble(), 1e-6);
  QCOMPARE(f.resolution().valueDouble(), 1e-6);

  // Decoding CONF? query answer
  data = "VOLT:DC +1.000000E+01,+1.000000E-06\n";
  QVERIFY(f.decodeConfigure(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_VOLTAGE_DC);
  QVERIFY(f.measureRange().isValid());
  ///QCOMPARE(f.measureRange().toDouble(), 10.0);
  QCOMPARE(f.measureRange().valueDouble(), 10.0);
  QVERIFY(f.resolution().isValid());
  ///QCOMPARE(f.resolution().toDouble(), 1e-6);
  QCOMPARE(f.resolution().valueDouble(), 1e-6);

  // Decoding CONF? query answer
  data = "VOLT:AC +1.000000E+01,+1.000000E-06\n";
  QVERIFY(f.decodeConfigure(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_VOLTAGE_AC);
  QVERIFY(f.measureRange().isValid());
  ///QCOMPARE(f.measureRange().toDouble(), 10.0);
  QCOMPARE(f.measureRange().valueDouble(), 10.0);
  QVERIFY(f.resolution().isValid());
  ///QCOMPARE(f.resolution().toDouble(), 1e-6);
  QCOMPARE(f.resolution().valueDouble(), 1e-6);

  // Decoding CONF? query answer
  data = " CURR +1.000000E-01,+1.000000E-05\n";
  QVERIFY(f.decodeConfigure(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_CURRENT_DC);
  QVERIFY(f.measureRange().isValid());
  ///QCOMPARE(f.measureRange().toDouble(), 0.1);
  QCOMPARE(f.measureRange().valueDouble(), 0.1);
  QVERIFY(f.resolution().isValid());
  ///QCOMPARE(f.resolution().toDouble(), 1e-5);
  QCOMPARE(f.resolution().valueDouble(), 1e-5);

  // Decoding CONF? query answer
  data = " CURR:DC +1.000000E-01,+1.000000E-05\n";
  QVERIFY(f.decodeConfigure(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_CURRENT_DC);
  QVERIFY(f.measureRange().isValid());
  ///QCOMPARE(f.measureRange().toDouble(), 0.1);
  QCOMPARE(f.measureRange().valueDouble(), 0.1);
  QVERIFY(f.resolution().isValid());
  ///QCOMPARE(f.resolution().toDouble(), 1e-5);
  QCOMPARE(f.resolution().valueDouble(), 1e-5);

  // Decoding CONF? query answer
  data = " CURR:AC +1.000000E-01,+1.000000E-05\n";
  QVERIFY(f.decodeConfigure(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_CURRENT_AC);
  QVERIFY(f.measureRange().isValid());
  ///QCOMPARE(f.measureRange().toDouble(), 0.1);
  QCOMPARE(f.measureRange().valueDouble(), 0.1);
  QVERIFY(f.resolution().isValid());
  ///QCOMPARE(f.resolution().toDouble(), 1e-5);
  QCOMPARE(f.resolution().valueDouble(), 1e-5);

  // Decoding FUNC? query answer
  data = "VOLT:AC\n";
  QVERIFY(f.decodeConfigure(data));
  QVERIFY(f.measureType() == mdtFrameCodecScpiU3606A::MT_VOLTAGE_AC);
  QVERIFY(!f.measureRange().isValid());
  QVERIFY(!f.resolution().isValid());

  // Calibration secure string
  /**
  data = "CAL: 27 Nov 2009\n";
  QVERIFY(f.decodeConfFunc(data));
  QVERIFY(f.values().size() == 2);
  QVERIFY(f.values().at(0) == "CAL");
  QVERIFY(f.values().at(1) == " 27 Nov 2009");
  */

}

void mdtFrameCodecTest::mdtFrameCodecModbusTest()
{
  mdtFrameCodecModbus c;
  QByteArray pdu;
  ///QList<quint16> aValues;
  QList<int> aValues;
  QList<bool> dValues;

  // Initial states
  QVERIFY(c.values().size() == 0);

  // Check encode of ReadCoils
  pdu = c.encodeReadCoils(0x0203, 0x0408);
  QVERIFY(pdu.size() == 5);
  QVERIFY(pdu.at(0) == 0x01); // Function code
  QVERIFY(pdu.at(1) == 0x02); // Start address H
  QVERIFY(pdu.at(2) == 0x03); // Start address L
  QVERIFY(pdu.at(3) == 0x04); // Coils count H
  QVERIFY(pdu.at(4) == 0x08); // Coils count L

  // Check decode of ReadCoils
  pdu.clear();
  pdu.append(0x01);   // Function code
  pdu.append(3);      // Bytes count
  pdu.append(0x07);   // 0000 0111
  pdu.append(0x45);   // 0100 0101
  pdu.append(0x81);   // 1000 0001
  QVERIFY(c.decode(pdu) == 0x01);
  QVERIFY(c.values().size() == 24);
  QVERIFY(c.values().at(0).toBool() == true);
  QVERIFY(c.values().at(1).toBool() == true);
  QVERIFY(c.values().at(2).toBool() == true);
  QVERIFY(c.values().at(3).toBool() == false);
  QVERIFY(c.values().at(4).toBool() == false);
  QVERIFY(c.values().at(5).toBool() == false);
  QVERIFY(c.values().at(6).toBool() == false);
  QVERIFY(c.values().at(7).toBool() == false);
  QVERIFY(c.values().at(8).toBool() == true);
  QVERIFY(c.values().at(9).toBool() == false);
  QVERIFY(c.values().at(10).toBool() == true);
  QVERIFY(c.values().at(11).toBool() == false);
  QVERIFY(c.values().at(12).toBool() == false);
  QVERIFY(c.values().at(13).toBool() == false);
  QVERIFY(c.values().at(14).toBool() == true);
  QVERIFY(c.values().at(15).toBool() == false);
  QVERIFY(c.values().at(16).toBool() == true);
  QVERIFY(c.values().at(17).toBool() == false);
  QVERIFY(c.values().at(18).toBool() == false);
  QVERIFY(c.values().at(19).toBool() == false);
  QVERIFY(c.values().at(20).toBool() == false);
  QVERIFY(c.values().at(21).toBool() == false);
  QVERIFY(c.values().at(22).toBool() == false);
  QVERIFY(c.values().at(23).toBool() == true);

  // Check decode of ReadCoils with errors in PDU
  pdu.clear();
  pdu.append(0x01);   // Function code
  pdu.append(3);      // Bytes count
  pdu.append(0x07);   // 0000 0111
  pdu.append(0x45);   // 1000 0101
  QVERIFY(c.decode(pdu) < 0);
  QVERIFY(c.values().size() == 0);
  pdu.clear();
  pdu.append(0x01);   // Function code
  pdu.append(1);      // Bytes count
  pdu.append(0x07);   // 0000 0111
  pdu.append(0x45);   // 1000 0101
  QVERIFY(c.decode(pdu) < 0);
  QVERIFY(c.values().size() == 0);

  // Encode ReadDiscreteInputs (FC 02 , 0x02)
  pdu = c.encodeReadDiscreteInputs(0x0102, 0x0304);
  QVERIFY(pdu.size() == 5);
  QVERIFY((quint8)pdu.at(0) == 0x02);   // Function code
  QVERIFY((quint8)pdu.at(1) == 0x01);   // Start address H
  QVERIFY((quint8)pdu.at(2) == 0x02);   // Start address L
  QVERIFY((quint8)pdu.at(3) == 0x03);   // Inputs count H
  QVERIFY((quint8)pdu.at(4) == 0x04);   // Inputs count L

  // Decode ReadDiscreteInputs (FC 02 , 0x02)
  pdu.clear();
  // Check with 3 inputs
  pdu.append(0x02);   // Function code
  pdu.append(1);      // Bytes count
  pdu.append(0x05);   // 0000 0101
  QVERIFY(c.decode(pdu) == 0x02);
  // The exactly amount of inputs is unknown, we only know the bytes count
  QVERIFY(c.values().size() == 8);
  QVERIFY(c.values().at(0) == true);
  QVERIFY(c.values().at(1) == false);
  QVERIFY(c.values().at(2) == true);
  QVERIFY(c.values().at(3) == false);
  QVERIFY(c.values().at(4) == false);
  QVERIFY(c.values().at(5) == false);
  QVERIFY(c.values().at(6) == false);
  QVERIFY(c.values().at(7) == false);

  // Encode ReadHoldingRegisters (FC 03 , 0x03)
  pdu = c.encodeReadHoldingRegisters(0x0105, 0x0051);
  QVERIFY(pdu.size() == 5);
  QVERIFY((quint8)pdu.at(0) == 0x03);   // Function code
  QVERIFY((quint8)pdu.at(1) == 0x01);   // Start address H
  QVERIFY((quint8)pdu.at(2) == 0x05);   // Start address L
  QVERIFY((quint8)pdu.at(3) == 0x00);   // Registers count H
  QVERIFY((quint8)pdu.at(4) == 0x51);   // Registers count L

  // Decode ReadHoldingRegisters (FC 03 , 0x03)
  pdu.clear();
  pdu.append(0x03);   // Function code
  pdu.append(4);      // Bytes count
  pdu.append(0x05);   // Register H
  pdu.append(0x49);   // Register L
  pdu.append(0x01);   // Register H
  pdu.append(0x07);   // Register L
  QVERIFY(c.decode(pdu) == 0x03);
  QVERIFY(c.values().size() == 2);
  QVERIFY(c.values().at(0) == 0x0549);
  QVERIFY(c.values().at(1) == 0x0107);

  // Encode ReadInputRegisters (FC 04 , 0x04)
  pdu = c.encodeReadInputRegisters(0x0175, 0x0051);
  QVERIFY(pdu.size() == 5);
  QVERIFY((quint8)pdu.at(0) == 0x04);   // Function code
  QVERIFY((quint8)pdu.at(1) == 0x01);   // Start address H
  QVERIFY((quint8)pdu.at(2) == 0x75);   // Start address L
  QVERIFY((quint8)pdu.at(3) == 0x00);   // Registers count H
  QVERIFY((quint8)pdu.at(4) == 0x51);   // Registers count L

  // Decode ReadInputRegisters (FC 04 , 0x04)
  pdu.clear();
  pdu.append(0x04);   // Function code
  pdu.append(6);      // Bytes count
  pdu.append(0x04);   // Register H
  pdu.append(0x47);   // Register L
  pdu.append(0x02);   // Register H
  pdu.append(0x03);   // Register L
  pdu.append(0x11);   // Register H
  pdu.append(0x56);   // Register L
  QVERIFY(c.decode(pdu) == 0x04);
  QVERIFY(c.values().size() == 3);
  QVERIFY(c.values().at(0) == 0x0447);
  QVERIFY(c.values().at(1) == 0x0203);
  QVERIFY(c.values().at(2) == 0x1156);

  // Check the encode of WriteSingleCoil
  pdu = c.encodeWriteSingleCoil(0x1258, true);
  QVERIFY(pdu.at(0) == 0x05);         // Function code
  QVERIFY(pdu.at(1) == 0x12);         // Address H
  QVERIFY(pdu.at(2) == 0x58);         // Address L
  QVERIFY((quint8)pdu.at(3) == 0xFF); // Value H
  QVERIFY(pdu.at(4) == 0x00);         // Value L

  // Check decode of WriteSingleCoil
  pdu.clear();
  pdu.append(0x05);     // Function code
  pdu.append(0x25);     // Address H
  pdu.append(0x06);     // Address L
  pdu.append(0xFF);     // Value H
  pdu.append((char)0);  // Value L
  QVERIFY(c.decode(pdu) == 0x05);
  QVERIFY(c.values().size() == 2);
  QVERIFY(c.values().at(0) == 0x2506);
  QVERIFY(c.values().at(1) == true);
  pdu.clear();
  pdu.append(0x05);     // Function code
  pdu.append(0x25);     // Address H
  pdu.append(0x06);     // Address L
  pdu.append((char)0);  // Value H
  pdu.append((char)0);  // Value L
  QVERIFY(c.decode(pdu) == 0x05);
  QVERIFY(c.values().size() == 2);
  QVERIFY(c.values().at(0) == 0x2506);
  QVERIFY(c.values().at(1) == false);

  // Encode WriteSingleRegister (FC 06 , 0x06)
  pdu = c.encodeWriteSingleRegister(0x123, 0x7542);
  QVERIFY(pdu.size() == 5);
  QVERIFY(pdu.at(0) == 0x06);
  QVERIFY((quint8)pdu.at(1) == 0x01);
  QVERIFY((quint8)pdu.at(2) == 0x23);
  QVERIFY((quint8)pdu.at(3) == 0x75);
  QVERIFY((quint8)pdu.at(4) == 0x42);

  // Check decode of WriteSingleRegister (FC 06 , 0x06)
  pdu.clear();
  pdu.append(0x06);     // Function code
  pdu.append(0x04);     // Address H
  pdu.append(0x12);     // Address L
  pdu.append(0x12);     // Value H
  pdu.append(0x85);     // Value L
  QVERIFY(c.decode(pdu) == 0x06);
  QVERIFY(c.values().size() == 2);
  QVERIFY(c.values().at(0) == 0x0412);
  QVERIFY(c.values().at(1) == 0x1285);

  // Encode WriteMultipleCoils (FC 15 , 0x0F)
  dValues.clear();
  // Check with 3 coils
  dValues << true << false << true;
  pdu = c.encodeWriteMultipleCoils(0x0143, dValues);
  QVERIFY(pdu.size() == 7);
  QVERIFY(pdu.at(0) == 0x0F);
  QVERIFY((quint8)pdu.at(1) == 0x01);
  QVERIFY((quint8)pdu.at(2) == 0x43);
  QVERIFY((quint8)pdu.at(3) == 0x00);
  QVERIFY((quint8)pdu.at(4) == 0x03);
  QVERIFY((quint8)pdu.at(5) == 0x01);
  QVERIFY((quint8)pdu.at(6) == 0x05);
  // Check with 8 coils
  dValues.clear();
  dValues << false << true << true << false << false << true << true << false;
  pdu = c.encodeWriteMultipleCoils(0x0140, dValues);
  QVERIFY(pdu.size() == 7);
  QVERIFY(pdu.at(0) == 0x0F);
  QVERIFY((quint8)pdu.at(1) == 0x01);
  QVERIFY((quint8)pdu.at(2) == 0x40);
  QVERIFY((quint8)pdu.at(3) == 0x00);
  QVERIFY((quint8)pdu.at(4) == 0x08);
  QVERIFY((quint8)pdu.at(5) == 0x01);
  QVERIFY((quint8)pdu.at(6) == 0x66);
  // Check with 9 coils
  dValues.clear();
  dValues << true << false << true << false << false << true << true << false << true;
  pdu = c.encodeWriteMultipleCoils(0x0241, dValues);
  QVERIFY(pdu.size() == 8);
  QVERIFY(pdu.at(0) == 0x0F);
  QVERIFY((quint8)pdu.at(1) == 0x02);
  QVERIFY((quint8)pdu.at(2) == 0x41);
  QVERIFY((quint8)pdu.at(3) == 0x00);
  QVERIFY((quint8)pdu.at(4) == 0x09);
  QVERIFY((quint8)pdu.at(5) == 0x02);
  QVERIFY((quint8)pdu.at(6) == 0x65);
  QVERIFY((quint8)pdu.at(7) == 0x01);

  // Check decode of WriteMultipleCoils (FC 15 , 0x0F)
  pdu.clear();
  pdu.append(0x0F);     // Function code
  pdu.append(0x07);     // Address H
  pdu.append(0x21);     // Address L
  pdu.append(0x02);     // Qty H
  pdu.append(0x12);     // Qty L
  QVERIFY(c.decode(pdu) == 0x0F);
  QVERIFY(c.values().size() == 2);
  QVERIFY(c.values().at(0) == 0x0721);
  QVERIFY(c.values().at(1) == 0x0212);

  // Encode WriteMultipleRegisters (FC 16 , 0x10)
  aValues.clear();
  aValues << 0x1256 << 0x8546;
  pdu = c.encodeWriteMultipleRegisters(0x0205, aValues);
  QVERIFY(pdu.size() == 10);
  QVERIFY(pdu.at(0) == 0x10);
  QVERIFY((quint8)pdu.at(1) == 0x02);
  QVERIFY((quint8)pdu.at(2) == 0x05);
  QVERIFY((quint8)pdu.at(3) == 0x00);
  QVERIFY((quint8)pdu.at(4) == 0x02);
  QVERIFY((quint8)pdu.at(5) == 0x04);
  // Values
  QVERIFY((quint8)pdu.at(6) == 0x12);
  QVERIFY((quint8)pdu.at(7) == 0x56);
  QVERIFY((quint8)pdu.at(8) == 0x85);
  QVERIFY((quint8)pdu.at(9) == 0x46);

  // Check decode of WriteMultipleRegisters (FC 16 , 0x10)
  pdu.clear();
  pdu.append(0x10);     // Function code
  pdu.append(0x06);     // Address H
  pdu.append(0x22);     // Address L
  pdu.append(0x02);     // Qty H
  pdu.append(0x75);     // Qty L
  QVERIFY(c.decode(pdu) == 0x10);
  QVERIFY(c.values().size() == 2);
  QVERIFY(c.values().at(0) == 0x0622);
  QVERIFY(c.values().at(1) == 0x0275);

  // Check encode of ReadDeviceIdentification - Object ID 0x00 (VendorName) + stream access
  pdu = c.encodeReadDeviceIdentification(0, false);
  QCOMPARE(pdu.size(), 4);
  QVERIFY((quint8)pdu.at(0) == 0x2B);
  QVERIFY((quint8)pdu.at(1) == 0x0E);
  QVERIFY((quint8)pdu.at(2) == 0x01);
  QVERIFY((quint8)pdu.at(3) == 0x00);

  // Check encode of ReadDeviceIdentification - Object ID 0x03 (VendorUrl) + stream access
  pdu = c.encodeReadDeviceIdentification(0x03, false);
  QCOMPARE(pdu.size(), 4);
  QVERIFY((quint8)pdu.at(0) == 0x2B);
  QVERIFY((quint8)pdu.at(1) == 0x0E);
  QVERIFY((quint8)pdu.at(2) == 0x02);
  QVERIFY((quint8)pdu.at(3) == 0x03);

  // Check encode of ReadDeviceIdentification - Object ID 0x80 (Product dependent) + stream access
  pdu = c.encodeReadDeviceIdentification(0x80, false);
  QCOMPARE(pdu.size(), 4);
  QVERIFY((quint8)pdu.at(0) == 0x2B);
  QVERIFY((quint8)pdu.at(1) == 0x0E);
  QVERIFY((quint8)pdu.at(2) == 0x03);
  QVERIFY((quint8)pdu.at(3) == 0x80);

  // Check encode of ReadDeviceIdentification - Object ID 0x01 (ProductCode) + individual access
  pdu = c.encodeReadDeviceIdentification(1, true);
  QCOMPARE(pdu.size(), 4);
  QVERIFY((quint8)pdu.at(0) == 0x2B);
  QVERIFY((quint8)pdu.at(1) == 0x0E);
  QVERIFY((quint8)pdu.at(2) == 0x04);
  QVERIFY((quint8)pdu.at(3) == 0x01);

  // Check encode of ReadDeviceIdentification - Object ID 0x04 (ProductName) + individual access
  pdu = c.encodeReadDeviceIdentification(4, true);
  QCOMPARE(pdu.size(), 4);
  QVERIFY((quint8)pdu.at(0) == 0x2B);
  QVERIFY((quint8)pdu.at(1) == 0x0E);
  QVERIFY((quint8)pdu.at(2) == 0x04);
  QVERIFY((quint8)pdu.at(3) == 0x04);

  // Check encode of ReadDeviceIdentification - Object ID 0x81 (Product dependent) + individual access
  pdu = c.encodeReadDeviceIdentification(0x81, true);
  QCOMPARE(pdu.size(), 4);
  QVERIFY((quint8)pdu.at(0) == 0x2B);
  QVERIFY((quint8)pdu.at(1) == 0x0E);
  QVERIFY((quint8)pdu.at(2) == 0x04);
  QVERIFY((quint8)pdu.at(3) == 0x81);

  // Check decode of ReadDeviceIdentification - Stream, 1 transaction
  pdu.clear();
  pdu.append(0x2B);         // Function code
  pdu.append(0x0E);         // MEI Type
  pdu.append(0x01);         // Read Dev Id Code
  pdu.append(0x01);         // Conformity level
  pdu.append((char)0x00);   // More Follows
  pdu.append((char)0x00);   // Next Object Id
  pdu.append(0x03);         // Number of Objects
  pdu.append((char)0x00);   // Object Id
  pdu.append(0x08);         // Object Length
  pdu.append("Fake Cpy");   // Object Value
  pdu.append(0x01);         // Object Id
  pdu.append(0x0A);         // Object Length
  pdu.append("Product xy"); // Object Value
  pdu.append(0x02);         // Object Id
  pdu.append(0x05);         // Object Length
  pdu.append("V2.11");      // Object Value
  QVERIFY(c.decode(pdu) == 0x2B);
  QVERIFY(c.values().size() == 8);
  QVERIFY(c.values().at(0) == false);
  QVERIFY(c.values().at(1) == 0x00);
  QVERIFY(c.values().at(2) == 0x00);
  QVERIFY(c.values().at(3) == "Fake Cpy");
  QVERIFY(c.values().at(4) == 0x01);
  QVERIFY(c.values().at(5) == "Product xy");
  QVERIFY(c.values().at(6) == 0x02);
  QVERIFY(c.values().at(7) == "V2.11");

  // Check decode of ReadDeviceIdentification - Stream, 2 transactions
  pdu.clear();
  pdu.append(0x2B);         // Function code
  pdu.append(0x0E);         // MEI Type
  pdu.append(0x01);         // Read Dev Id Code
  pdu.append(0x01);         // Conformity level
  pdu.append(0xFF);         // More Follows
  pdu.append(0x02);         // Next Object Id
  pdu.append(0x03);         // Number of Objects
  pdu.append((char)0x00);   // Object Id
  pdu.append(0x05);         // Object Length
  pdu.append("A Cpy");      // Object Value
  pdu.append(0x01);         // Object Id
  pdu.append(0x09);         // Object Length
  pdu.append("Product z");  // Object Value
  QVERIFY(c.decode(pdu) == 0x2B);
  QVERIFY(c.values().size() == 6);
  QVERIFY(c.values().at(0) == true);
  QVERIFY(c.values().at(1) == 0x02);
  QVERIFY(c.values().at(2) == 0x00);
  QVERIFY(c.values().at(3) == "A Cpy");
  QVERIFY(c.values().at(4) == 0x01);
  QVERIFY(c.values().at(5) == "Product z");
  pdu.clear();
  pdu.append(0x2B);         // Function code
  pdu.append(0x0E);         // MEI Type
  pdu.append(0x01);         // Read Dev Id Code
  pdu.append(0x01);         // Conformity level
  pdu.append((char)0x00);   // More Follows
  pdu.append((char)0x00);   // Next Object Id
  pdu.append(0x03);         // Number of Objects
  pdu.append(0x02);         // Object Id
  pdu.append(0x07);         // Object Length
  pdu.append("Version");    // Object Value
  QVERIFY(c.decode(pdu) == 0x2B);
  QVERIFY(c.values().size() == 4);
  QVERIFY(c.values().at(0) == false);
  QVERIFY(c.values().at(1) == 0x00);
  QVERIFY(c.values().at(2) == 0x02);
  QVERIFY(c.values().at(3) == "Version");

  // Check decode of ReadDeviceIdentification - Stream, 1 transaction, no object
  pdu.clear();
  pdu.append(0x2B);         // Function code
  pdu.append(0x0E);         // MEI Type
  pdu.append(0x01);         // Read Dev Id Code
  pdu.append(0x01);         // Conformity level
  pdu.append((char)0x00);   // More Follows
  pdu.append((char)0x00);   // Next Object Id
  pdu.append((char)0x00);   // Number of Objects
  QVERIFY(c.decode(pdu) < 0);
  QVERIFY(c.values().size() == 0);
  // Check decode of ReadDeviceIdentification - Stream, 1 transaction, Wrong object length
  pdu.clear();
  pdu.append(0x2B);         // Function code
  pdu.append(0x0E);         // MEI Type
  pdu.append(0x01);         // Read Dev Id Code
  pdu.append(0x01);         // Conformity level
  pdu.append((char)0x00);   // More Follows
  pdu.append((char)0x00);   // Next Object Id
  pdu.append(0x01);         // Number of Objects
  pdu.append((char)0x00);   // Object Id
  pdu.append((char)0x00);   // Object Length
  pdu.append("Err Cpy");    // Object Value
  QVERIFY(c.decode(pdu) < 0);
  QVERIFY(c.values().size() == 0);
  // Check decode of ReadDeviceIdentification - Stream, 1 transaction, Wrong object length
  pdu.clear();
  pdu.append(0x2B);         // Function code
  pdu.append(0x0E);         // MEI Type
  pdu.append(0x01);         // Read Dev Id Code
  pdu.append(0x01);         // Conformity level
  pdu.append((char)0x00);   // More Follows
  pdu.append((char)0x00);   // Next Object Id
  pdu.append(0x01);         // Number of Objects
  pdu.append((char)0x00);   // Object Id
  pdu.append((char)0xDF);   // Object Length
  pdu.append("Err Cpy");    // Object Value
  QVERIFY(c.decode(pdu) < 0);
  QVERIFY(c.values().size() == 0);

}

/*
  k8055 frame formats, taken from libk8055:
   http://libk8055.sourceforge.net/

  Input packet format
  +---+---+---+---+---+---+---+---+
  |DIn|Sta|A1 |A2 |   C1  |   C2  |
  +---+---+---+---+---+---+---+---+
  DIn = Digital input in high nibble, except for input 3 in 0x01
  Sta = Status,x01 = OK ?
  A1  = Analog input 1, 0-255
  A2  = Analog input 2, 0-255
  C1  = Counter 1, 16 bits (lsb)
  C2  = Counter 2, 16 bits (lsb)

  Output packet format
  +---+---+---+---+---+---+---+---+
  |CMD|DIG|An1|An2|Rs1|Rs2|Dbv|Dbv|
  +---+---+---+---+---+---+---+---+
  CMD = Command 
  DIG = Digital output bitmask
  An1 = Analog output 1 value, 0-255
  An2 = Analog output 2 value, 0-255
  Rs1 = Reset counter 1, command 3
  Rs2 = Reset counter 3, command 4
  Dbv = Debounce value for counter 1 and 2, command 1 and 2

  Or split by commands

  Cmd 0, Reset ??
  Cmd 1, Set debounce Counter 1
  +---+---+---+---+---+---+---+---+
  |CMD|   |   |   |   |   |Dbv|   |
  +---+---+---+---+---+---+---+---+
  Cmd 2, Set debounce Counter 2
  +---+---+---+---+---+---+---+---+
  |CMD|   |   |   |   |   |   |Dbv|
  +---+---+---+---+---+---+---+---+
  Cmd 3, Reset counter 1
  +---+---+---+---+---+---+---+---+
  | 3 |   |   |   | 00|   |   |   |
  +---+---+---+---+---+---+---+---+
  Cmd 4, Reset counter 2
  +---+---+---+---+---+---+---+---+
  | 4 |   |   |   |   | 00|   |   |
  +---+---+---+---+---+---+---+---+
  cmd 5, Set analog/digital
  +---+---+---+---+---+---+---+---+
  | 5 |DIG|An1|An2|   |   |   |   |
  +---+---+---+---+---+---+---+---+
*/
void mdtFrameCodecTest::mdtFrameCodecK8055Test()
{
  mdtFrameCodecK8055 codec;
  QByteArray frame;

  /*
   * Initial states
   */
  QVERIFY(codec.values().size() == 0);

  /*
   * Decode test
   */
  frame.clear();
  // Set input 1, 3, 5 to 1
  frame.append((char)0x91);
  // Set status to 1
  frame.append((char)0x01);
  // Set analog 1 to 89
  frame.append((char)0x59);
  // Set analog 2 to 253
  frame.append((char)0xFD);
  // Set counter 1 to 260
  frame.append((char)0x01);
  frame.append((char)0x04);
  // Set counter 2 to 4512
  frame.append((char)0x11);
  frame.append((char)0xA0);
  // Decode and check values
  QVERIFY(codec.decode(frame));
  QCOMPARE(codec.values().size() , 9);
  QVERIFY(codec.values().at(0).type() == QVariant::Bool);
  QVERIFY(codec.values().at(0) == true);
  QVERIFY(codec.values().at(1).type() == QVariant::Bool);
  QVERIFY(codec.values().at(1) == false);
  QVERIFY(codec.values().at(2).type() == QVariant::Bool);
  QVERIFY(codec.values().at(2) == true);
  QVERIFY(codec.values().at(3).type() == QVariant::Bool);
  QVERIFY(codec.values().at(3) == false);
  QVERIFY(codec.values().at(4).type() == QVariant::Bool);
  QVERIFY(codec.values().at(4) == true);
  /**
  QVERIFY(codec.values().at(5).type() == QVariant::Bool);
  QVERIFY(codec.values().at(5) == true);
  QVERIFY(codec.values().at(6).type() == QVariant::Bool);
  QVERIFY(codec.values().at(6) == false);
  QVERIFY(codec.values().at(7).type() == QVariant::Bool);
  QVERIFY(codec.values().at(7) == false);
  QVERIFY(codec.values().at(8).type() == QVariant::Int);
  QVERIFY(codec.values().at(8) == 89);
  */
  QVERIFY(codec.values().at(5).type() == QVariant::Int);
  QVERIFY(codec.values().at(5) == 89);
  QVERIFY(codec.values().at(6).type() == QVariant::Int);
  QVERIFY(codec.values().at(6) == 253);
  QVERIFY(codec.values().at(7).type() == QVariant::Int);
  QVERIFY(codec.values().at(7) == 260);
  QVERIFY(codec.values().at(8).type() == QVariant::Int);
  QVERIFY(codec.values().at(8) == 4512);

  /*
   * Encode reset
   */
  frame.clear();
  frame = codec.encodeReset();
  QVERIFY(frame.size() == 8);
  QVERIFY(frame.at(0) == 0x00);
  QVERIFY(frame.at(1) == 0x00);
  QVERIFY(frame.at(2) == 0x00);
  QVERIFY(frame.at(3) == 0x00);
  QVERIFY(frame.at(4) == 0x00);
  QVERIFY(frame.at(5) == 0x00);
  QVERIFY(frame.at(6) == 0x00);
  QVERIFY(frame.at(7) == 0x00);

  /*
   * Encode set counter debounce value
   */
  frame.clear();
  frame = codec.encodeSetCounterDebounceValue(1, 128);
  QVERIFY(frame.size() == 8);
  QVERIFY((quint8)frame.at(0) == 0x01);
  QVERIFY((quint8)frame.at(1) == 0x00);
  QVERIFY((quint8)frame.at(2) == 0x00);
  QVERIFY((quint8)frame.at(3) == 0x00);
  QVERIFY((quint8)frame.at(4) == 0x00);
  QVERIFY((quint8)frame.at(5) == 0x00);
  QVERIFY((quint8)frame.at(6) == 128);
  QVERIFY((quint8)frame.at(7) == 0x00);
  frame.clear();
  frame = codec.encodeSetCounterDebounceValue(2, 233);
  QVERIFY(frame.size() == 8);
  QVERIFY((quint8)frame.at(0) == 0x01);
  QVERIFY((quint8)frame.at(1) == 0x00);
  QVERIFY((quint8)frame.at(2) == 0x00);
  QVERIFY((quint8)frame.at(3) == 0x00);
  QVERIFY((quint8)frame.at(4) == 0x00);
  QVERIFY((quint8)frame.at(5) == 0x00);
  QVERIFY((quint8)frame.at(6) == 0x00);
  QVERIFY((quint8)frame.at(7) == 233);

  /*
   * Encode reset counter
   */
  frame.clear();
  frame = codec.encodeResetCounter(1);
  QVERIFY(frame.size() == 8);
  QVERIFY((quint8)frame.at(0) == 0x03);
  frame.clear();
  frame = codec.encodeResetCounter(2);
  QVERIFY(frame.size() == 8);
  QVERIFY((quint8)frame.at(0) == 0x04);

  /*
   * Encode Analog/Digital output
   */
  frame.clear();
  codec.setDigitalOut(1, true);
  codec.setDigitalOut(2, true);
  codec.setDigitalOut(7, true);
  codec.setAnalogOut(1, 57);
  codec.setAnalogOut(2, 254);
  frame = codec.encodeSetOutputs();
  QVERIFY(frame.size() == 8);
  QVERIFY((quint8)frame.at(0) == 0x05); // Command 5: set Douts + Aouts
  QVERIFY((quint8)frame.at(1) == 0x43); // Dout bitmask
  QVERIFY((quint8)frame.at(2) == 57);   // Aout1 value
  QVERIFY((quint8)frame.at(3) == 254);  // Aout2 value
  QVERIFY((quint8)frame.at(4) == 0x00); // Reset counter 1
  QVERIFY((quint8)frame.at(5) == 0x00); // Reset counter 2
  QVERIFY((quint8)frame.at(6) == 0x00); // Debounce value for counter 1
  QVERIFY((quint8)frame.at(7) == 0x00); // Debounce value for counter 2

  codec.setDigitalOut(1, true);
  codec.setDigitalOut(2, false);
  codec.setDigitalOut(3, true);
  codec.setDigitalOut(4, false);
  codec.setDigitalOut(5, true);
  codec.setDigitalOut(6, false);
  codec.setDigitalOut(7, true);
  codec.setDigitalOut(8, false);
  codec.setAnalogOut(1, 255);
  codec.setAnalogOut(2, 0);
  frame = codec.encodeSetOutputs();
  QCOMPARE(frame.size() , 8);
  QCOMPARE((int)((quint8)frame.at(0)) , 0x05); // Command 5: set Douts + Aouts
  QCOMPARE((int)((quint8)frame.at(1)) , 0x55); // Dout bitmask
  QCOMPARE((int)((quint8)frame.at(2)) , 255);  // Aout1 value
  QCOMPARE((int)((quint8)frame.at(3)) , 0);    // Aout2 value
  QCOMPARE((int)((quint8)frame.at(4)) , 0x00); // Reset counter 1
  QCOMPARE((int)((quint8)frame.at(5)) , 0x00); // Reset counter 2
  QCOMPARE((int)((quint8)frame.at(6)) , 0x00); // Debounce value for counter 1
  QCOMPARE((int)((quint8)frame.at(7)) , 0x00); // Debounce value for counter 2

  codec.setDigitalOut(1, false);
  codec.setDigitalOut(2, true);
  codec.setDigitalOut(3, false);
  codec.setDigitalOut(4, true);
  codec.setDigitalOut(5, false);
  codec.setDigitalOut(6, true);
  codec.setDigitalOut(7, false);
  codec.setDigitalOut(8, true);
  codec.setAnalogOut(1, 255);
  codec.setAnalogOut(2, 0);
  frame = codec.encodeSetOutputs();
  QCOMPARE(frame.size() , 8);
  QCOMPARE((int)((quint8)frame.at(0)) , 0x05); // Command 5: set Douts + Aouts
  QCOMPARE((int)((quint8)frame.at(1)) , 0xAA); // Dout bitmask
  QCOMPARE((int)((quint8)frame.at(2)) , 255);  // Aout1 value
  QCOMPARE((int)((quint8)frame.at(3)) , 0);    // Aout2 value
  QCOMPARE((int)((quint8)frame.at(4)) , 0x00); // Reset counter 1
  QCOMPARE((int)((quint8)frame.at(5)) , 0x00); // Reset counter 2
  QCOMPARE((int)((quint8)frame.at(6)) , 0x00); // Debounce value for counter 1
  QCOMPARE((int)((quint8)frame.at(7)) , 0x00); // Debounce value for counter 2

}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtFrameCodecTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
