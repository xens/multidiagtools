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
#include "mdtTcpSocketTest.h"
#include "mdtTcpSocket.h"
#include "mdtTcpSocketThread.h"
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <QTcpSocket>
#include "mdtTcpServer.h"
#include "mdtApplication.h"

#include "mdtFrameCodecModbus.h"
#include "mdtFrameModbusTcp.h"

#include <QDebug>

void mdtTcpSocketTest::essais()
{
  QSKIP("essais", SkipSingle);

  mdtTcpSocket s;
  mdtPortConfig cfg;
  mdtTcpSocketThread thd;
  mdtFrameModbusTcp *frame;
  mdtFrameCodecModbus mc;

  // Setup
  cfg.setFrameType(mdtFrame::FT_MODBUS_TCP);
  s.setConfig(&cfg);
  QVERIFY(s.setup() == mdtAbstractPort::NoError);

  // Assign socket to the thread
  thd.setPort(&s);

  // Start
  thd.start();
  thd.waitReady();
  QVERIFY(thd.isRunning());

  // Init connection
  ///s.connectToHost("192.168.1.102" , 502);

  // Send data to server
  s.lockMutex();
  QVERIFY(s.writeFramesPool().size() > 0);
  // Get a frame
  frame = dynamic_cast<mdtFrameModbusTcp*> (s.writeFramesPool().dequeue());
  QVERIFY(frame != 0);
  // Add some data to frame and commit
  ///frame->clear();
  ///frame->append(queries.at(i).toAscii());
  ///frame->setPdu(mc.encodeReadCoils(0x200, 1));
  frame->setPdu(mc.encodeWriteSingleCoil(0x01, true));
  frame->encode();
  qDebug() << "Sending frame: " << frame->toHex();
  for(int i=0; i<frame->size(); i++){
    qDebug() << "frame[" << i << "]: 0x" << hex << (quint8)frame->at(i);
  }
  ///s.writeFrames().enqueue(frame);
  ///s.unlockMutex();
  ///s.beginNewTransaction();
  s.addFrameToWrite(frame);
  s.unlockMutex();

  // Wait some time and verify that data was exchanged
  QTest::qWait(100);

  // Check received data
  s.lockMutex();
  QVERIFY(s.readenFrames().size() > 0);
  // Get a frame
  frame = dynamic_cast<mdtFrameModbusTcp*> (s.readenFrames().dequeue());
  QVERIFY(frame != 0);
  qDebug() << "Received frame: " << frame->toHex();
  for(int i=0; i<frame->size(); i++){
    qDebug() << "frame[" << i << "]: 0x" << hex << (quint8)frame->at(i);
  }
  qDebug() << "RESP FC: " << mc.decode(frame->getPdu());

  s.unlockMutex();

  // End
  thd.stop();
  thd.waitFinished();
}

void mdtTcpSocketTest::writeReadTest()
{
  mdtTcpSocket s;
  mdtPortConfig cfg;
  mdtTcpSocketThread thd;
  mdtFrame *frame;
  mdtTcpServer tcpServer;
  QStringList responses;
  QString peerPort;

  // Get data set
  QFETCH(QStringList, queries);
  QFETCH(QStringList, responsesRef);

  // Init the tcp server
  QVERIFY(tcpServer.listen());

  // Setup
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setReadQueueSize(10);
  cfg.setReadFrameSize(10000);
  cfg.setWriteQueueSize(10);
  cfg.setReadTimeout(5000);
  cfg.setWriteTimeout(5000);
  cfg.setEndOfFrameSeq('*');
  s.setConfig(&cfg);
  peerPort.setNum(tcpServer.serverPort());
  s.setPortName("127.0.0.1:" + peerPort);
  QVERIFY(s.open() == mdtAbstractPort::NoError);
  QVERIFY(s.setup() == mdtAbstractPort::NoError);

  // Assign socket to the thread
  thd.setPort(&s);

  // Start
  thd.start();
  thd.waitReady();
  QVERIFY(thd.isRunning());

  // Responses comming from server
  responses = responsesRef;
  // Add the EOF sequence
  for(int i=0; i<responses.size(); i++){
    qDebug() << "responses[" << i << "]: " << responses.at(i).size();
    responses[i].append("*");
  }

  // Set TCP server data
  tcpServer.setResponseData(responses);
  QVERIFY(tcpServer.isListening());

  // Send data to server
  s.lockMutex();
  QVERIFY(s.writeFramesPool().size() >= queries.size());
  s.unlockMutex();
  for(int i=0; i<queries.size(); i++){
    s.lockMutex();
    // Get a frame
    frame = s.writeFramesPool().dequeue();
    QVERIFY(frame != 0);
    // Add some data to frame and commit
    frame->clear();
    frame->append(queries.at(i).toLocal8Bit());
    s.addFrameToWrite(frame);
    s.unlockMutex();
  }
  s.unlockMutex();

  // Wait some time and verify that data was exchanged
  QTest::qWait(100);

  // Check received data
  s.lockMutex();
  QCOMPARE(s.readenFrames().size(), responses.size());
  for(int i=0; i<responses.size(); i++){
    // Get a frame
    frame = s.readenFrames().dequeue();
    QVERIFY(frame != 0);
    QVERIFY(*frame == responsesRef.at(i));
  }
  s.unlockMutex();

  // Check that all works again after a flush
  s.flush();

  // Set TCP server data
  tcpServer.setResponseData(responses);
  QVERIFY(tcpServer.isListening());

  // Send data to server
  s.lockMutex();
  QVERIFY(s.writeFramesPool().size() >= queries.size());
  s.unlockMutex();
  for(int i=0; i<queries.size(); i++){
    s.lockMutex();
    // Get a frame
    frame = s.writeFramesPool().dequeue();
    QVERIFY(frame != 0);
    // Add some data to frame and commit
    frame->clear();
    frame->append(queries.at(i).toLocal8Bit());
    s.addFrameToWrite(frame);
    s.unlockMutex();
  }
  s.unlockMutex();

  // Wait some time and verify that data was exchanged
  QTest::qWait(100);

  // Check received data
  s.lockMutex();
  QCOMPARE(s.readenFrames().size(), responses.size());
  for(int i=0; i<responses.size(); i++){
    // Get a frame
    frame = s.readenFrames().dequeue();
    QVERIFY(frame != 0);
    QVERIFY(*frame == responsesRef.at(i));
  }
  s.unlockMutex();

  // End
  thd.stop();
  thd.waitFinished();
}

void mdtTcpSocketTest::writeReadTest_data()
{
  QStringList q;
  QStringList r;
  QString f1, f2, f3;

  QTest::addColumn<QStringList>("queries");
  QTest::addColumn<QStringList>("responsesRef");

  q.clear();
  r.clear();
  q << "";
  r << "";
  QTest::newRow("Empty data") << q << r;

  q.clear();
  r.clear();
  q << "A";
  r << "1";
  QTest::newRow("1 char data") << q << r;

  q.clear();
  r.clear();
  q << "ABCD";
  r << "1234";
  QTest::newRow("1 frame short data") << q << r;

  q.clear();
  r.clear();
  q << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  r << "1234567890";
  QTest::newRow("1 frame middle len data") << q << r;

  q.clear();
  r.clear();
  q << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  r << "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  QTest::newRow("1 frame long data") << q << r;

  q.clear();
  r.clear();
  q << "A" << "B" << "C" << "D";
  r << "Z" << "8" << "G" << "4";
  QTest::newRow("Multi frame short data") << q << r;

  q.clear();
  r.clear();
  q << "ABCD" << "1234" << "abcd";
  r << "9876" << "FGHI" << "askjdf";
  QTest::newRow("Multi frame middle len data") << q << r;

  q.clear();
  r.clear();
  q << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << "0123456789" << "abcdefghijklmnopqrstuvwxyz";
  r << "0123456789" << "ABCDEFGHIJK0055LMNOPQRSTUVWXYZ" << "abcdefghijklmnopqrstuvwxyz8545";
  QTest::newRow("Multi frame long data") << q << r;

  q.clear();
  r.clear();
  for(int i=0; i<100; i++){
    f1 += "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789abcdefghijklmnopqrstuvwxyz";
    f2 += "abcdefghijklmnopqrstuvwxyz";
    f3 += "0123456789";
  }
  q << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << "0123456789" << "abcdefghijklmnopqrstuvwxyz";
  r << f1 << f2 << f3;
  QTest::newRow("Multi frame very long data") << q << r;
}

void mdtTcpSocketTest::readInvalidDataTest()
{
  mdtTcpSocket s;
  mdtPortConfig cfg;
  mdtTcpSocketThread thd;
  mdtFrame *frame;
  mdtTcpServer tcpServer;
  QStringList responses;
  QString peerPort;

  // Init the tcp server
  QVERIFY(tcpServer.listen());

  // Setup
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setReadQueueSize(10);
  cfg.setReadFrameSize(10);
  cfg.setWriteQueueSize(10);
  cfg.setReadTimeout(5000);
  cfg.setWriteTimeout(5000);
  cfg.setEndOfFrameSeq('*');
  s.setConfig(&cfg);
  peerPort.setNum(tcpServer.serverPort());
  s.setPortName("127.0.0.1:" + peerPort);
  QVERIFY(s.open() == mdtAbstractPort::NoError);
  QVERIFY(s.setup() == mdtAbstractPort::NoError);


  // Assign socket to the thread
  thd.setPort(&s);

  // Start
  thd.start();
  thd.waitReady();
  QVERIFY(thd.isRunning());

  /*
   * Data without EOF , size < capacity : must not crash, no frame must be readen
   */

  responses.clear();
  responses << "ABCDE";
  QVERIFY(responses.at(0).size() < cfg.readFrameSize());

  // Set TCP server data
  tcpServer.setResponseData(responses);
  QVERIFY(tcpServer.isListening());

  s.lockMutex();
  // Get a frame
  frame = s.writeFramesPool().dequeue();
  QVERIFY(frame != 0);
  // Add some data to frame and commit
  frame->clear();
  frame->append("");
  s.addFrameToWrite(frame);
  s.unlockMutex();

  // Wait some time and verify that data was exchanged
  QTest::qWait(100);

  // Check received data
  s.lockMutex();
  QVERIFY(s.readenFrames().size() == 0);
  s.unlockMutex();

  // Wait some time to enshure TCP server has finished ...
  tcpServer.close();
  QTest::qWait(100);

  // Stop and close port
  thd.stop();
  thd.waitFinished();
  QVERIFY(!thd.isRunning());
  s.close();

  /*
   * Data with EOF , size == capacity : must not crash, 1 frame must be readen
   */

  responses.clear();
  responses << "0123456789";
  QVERIFY(responses.at(0).size() == cfg.readFrameSize());

  QVERIFY(tcpServer.listen());

  // Open and start port
  peerPort.setNum(tcpServer.serverPort());
  s.setPortName("127.0.0.1:" + peerPort);
  QVERIFY(s.open() == mdtAbstractPort::NoError);
  QVERIFY(s.setup() == mdtAbstractPort::NoError);
  thd.start();
  thd.waitReady();
  QVERIFY(thd.isRunning());

  // Set TCP server data
  tcpServer.setResponseData(responses);

  s.lockMutex();
  // Get a frame
  frame = s.writeFramesPool().dequeue();
  QVERIFY(frame != 0);
  // Add some data to frame and commit
  frame->clear();
  frame->append("*");
  s.addFrameToWrite(frame);
  s.unlockMutex();

  // Wait some time and verify that data was exchanged
  QTest::qWait(100);

  // Check received data
  s.lockMutex();
  QVERIFY(s.readenFrames().size() == 1);
  // Get a frame
  frame = s.readenFrames().dequeue();
  QVERIFY(frame != 0);
  QVERIFY(*frame == responses.at(0));
  // Restore frame
  s.readFramesPool().enqueue(frame);
  s.unlockMutex();

  // Wait some time to enshure TCP server has finished ...
  tcpServer.close();
  QTest::qWait(100);

  // Stop and close port
  thd.stop();
  thd.waitFinished();
  QVERIFY(!thd.isRunning());
  s.close();

  /*
   * Data with EOF , size > capacity (but < 2*capacity) : must not crash, 1 frame must be readen
   */

  responses.clear();
  responses << "0123456789abcdefg";
  QVERIFY(responses.at(0).size() > cfg.readFrameSize());
  QVERIFY(responses.at(0).size() < 2*cfg.readFrameSize());

  QVERIFY(tcpServer.listen());

  // Open and start port
  peerPort.setNum(tcpServer.serverPort());
  s.setPortName("127.0.0.1:" + peerPort);
  QVERIFY(s.open() == mdtAbstractPort::NoError);
  QVERIFY(s.setup() == mdtAbstractPort::NoError);
  thd.start();
  thd.waitReady();
  QVERIFY(thd.isRunning());

  // Set TCP server data
  tcpServer.setResponseData(responses);

  s.lockMutex();
  // Get a frame
  frame = s.writeFramesPool().dequeue();
  QVERIFY(frame != 0);
  // Add some data to frame and commit
  frame->clear();
  frame->append("");
  s.addFrameToWrite(frame);
  s.unlockMutex();

  // Wait some time and verify that data was exchanged
  QTest::qWait(100);

  // Check received data
  s.lockMutex();
  QVERIFY(s.readenFrames().size() == 1);
  // Get a frame
  frame = s.readenFrames().dequeue();
  QVERIFY(frame != 0);
  QVERIFY(*frame == "0123456789");
  // Restore frame
  s.readFramesPool().enqueue(frame);
  s.unlockMutex();

  // End
  thd.stop();
  thd.waitFinished();
}

int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtTcpSocketTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
