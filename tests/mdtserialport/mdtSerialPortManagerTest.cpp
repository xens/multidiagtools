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
#include "mdtSerialPortManagerTest.h"
#include "mdtSerialPortManager.h"
#include "mdtSerialPort.h"
#include "mdtSerialPortConfig.h"
#include "mdtPortWriteThread.h"
#include "mdtPortReadThread.h"
#include "mdtSerialPortCtlThread.h"
#include "mdtApplication.h"
#include "mdtPortInfo.h"

#include <QtGlobal>
#include <QDateTime>
#include <QList>
#include <cstdio>

#include <QByteArray>
#include <QString>

void mdtSerialPortManagerTest::simpleTest()
{
  mdtSerialPortManager m;
  QList<mdtPortInfo*> portInfoList;
  QList<QByteArray> frames;

  qDebug() << "* Be shure that the computer has at least one serial port, else test will fail *";

  // Verify that scan() function works ..
  portInfoList = m.scan();
  if(portInfoList.size() < 1){
    QSKIP("No serial port found, or other error", SkipAll);
  }

  // Init port manager
  m.setKeepTransactionsDone(true);
  m.port().config().setFrameType(mdtFrame::FT_ASCII);
  m.port().config().setEndOfFrameSeq("$");
  m.setPortName(portInfoList.at(0)->portName());
  ///QVERIFY(m.openPort());

  // Check that available baud rates contains 9600 (very standard, should exists on most device)
  QCOMPARE((int)m.port().open(), 0);
  QVERIFY(m.port().availableBaudRates().size() > 0);
  QVERIFY(m.port().availableBaudRates().contains(9600));
  // Check that the right port is set
  QVERIFY(m.port().portName() == portInfoList.at(0)->portName());
  m.port().close();

  // We not need the scan result anymore, free memory
  qDeleteAll(portInfoList);
  portInfoList.clear();

  // Start port manager
  QVERIFY(m.start());
  ///QVERIFY(m.isRunning());
  QVERIFY(m.isReady());

  // Send some data
  ///QVERIFY(m.writeData("Test$") >= 0);
  QVERIFY(m.sendData("Test$") >= 0);

  // Wait until a transction is done
  QVERIFY(m.waitOneTransactionDone());

  // Verify received data
  frames = m.readenFrames();
  QVERIFY(frames.size() == 1);
  QVERIFY(frames.at(0) == "Test");
}

void mdtSerialPortManagerTest::transferTest()
{
  mdtSerialPortManager m;
  QList<mdtPortInfo*> portInfoList;
  QFETCH(QString, data);
  double dataTransferRate;
  int dataTransferTime;
  QString receivedData;
  QList<QByteArray> frames;

  qDebug() << "* Be shure that the test dongle is plugged in first serial port, else test will fail *";

  /*
   * RAW frames (stress test for port manager)
   */

  // Init port manager
  m.setKeepTransactionsDone(true);
  portInfoList = m.scan();
  if(portInfoList.size() < 1){
    QSKIP("No serial port found, or other error", SkipAll);
  }
  m.port().config().setFrameType(mdtFrame::FT_RAW);
  m.setPortName(portInfoList.at(0)->portName());
  ///QVERIFY(m.openPort());
  // Check that available baud rates contains 9600 (very standard, should exists on most device)
  QCOMPARE((int)m.port().open(), 0);
  QVERIFY(m.port().availableBaudRates().size() > 0);
  QVERIFY(m.port().availableBaudRates().contains(9600));
  m.port().close();
  // Default configuration is: 9600 baud, 8 data bits, no parity and 1 stop bit.
  dataTransferRate = (1.0/8.0)*9600.0*8.0/10.0; // [B/s]
  // Set the dataTransferTime, with 10% headroom
  dataTransferTime = 1000.0*1.1*(double)data.size() / dataTransferRate;

  // We not need the scan result anymore, free memory
  qDeleteAll(portInfoList);
  portInfoList.clear();

  // Start port manager
  QVERIFY(m.start());
  ///QVERIFY(m.isRunning());
  QVERIFY(m.isReady());

  qDebug() << "Data rate: " << dataTransferRate << " [B/s]";
  qDebug() << "data size: " << data.size();
  qDebug() << "Transfer time: " << dataTransferTime << " [ms]";

  // Send data
  ///QVERIFY(m.writeData(data.toAscii()) >= 0);
  QVERIFY(m.sendData(data.toLocal8Bit()) >= 0);

  // Get incomming data
  while(receivedData.size() < data.size()){
    // Wait until a transction is done
    QVERIFY(m.waitOneTransactionDone());
    // Copy data
    frames = m.readenFrames();
    for(int i=0; i<frames.size(); i++){
      receivedData += frames.at(i);
    }
  }
  // Verify received data
  QVERIFY(receivedData == data);

  ///m.closePort();
  m.stop();
}

void mdtSerialPortManagerTest::transferTest_data()
{
  QString str;

  QTest::addColumn<QString>("data");

  QTest::newRow("No data") << "";
  QTest::newRow("1 char data") << "a";
  QTest::newRow("small size data length") << "ABCDEFGH0123456789";
  // Generate a middle size data length
  str = "Hello ! abcdefgh";
  for(int i=0; i<5; i++){
    str += str;
  }
  QTest::newRow("mid size data length") << str;
  // Generate a big size data length
  str = "9876543210 / olleH ! abcdefgh ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  for(int i=0; i<8; i++){
    str += str;
  }
  QTest::newRow("big size data length") << str;
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtSerialPortManagerTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
