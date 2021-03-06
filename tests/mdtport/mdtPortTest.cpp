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
#include "mdtPortTest.h"
#include "mdtPort.h"
#include "mdtPortManager.h"
#include "mdtPortConfig.h"
#include "mdtPortReadThread.h"
#include "mdtPortWriteThread.h"
#include "mdtFrame.h"
#include <QTemporaryFile>
#include <QFileInfo>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include "mdtApplication.h"

//#include <QDebug>

void mdtPortTest::openCloseTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  QTemporaryFile file;
  QFileInfo fileInfo;

  // Initial state
  QVERIFY(!port.isOpen());

  // Create a temporary file
  QVERIFY(file.open());

  // Open port
  port.setPortName(file.fileName());
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());

#ifdef Q_OS_UNIX
  // Check about lock file
  fileInfo.setFile(file);
  ///qDebug() << "mdtPortTest::openCloseTest() , lock file: LCK.." << fileInfo.fileName();
  QVERIFY(QFile::exists("/tmp/LCK.." + fileInfo.fileName()));
  QVERIFY(QFile::exists("/var/lock/LCK.." + fileInfo.fileName()));
#endif

  // Setup
  port.setConfig(&cfg);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());

  // Close
  port.close();
  QVERIFY(!port.isOpen());

#ifdef Q_OS_UNIX
  // Check about lock file
  fileInfo.setFile(file);
  QVERIFY(!QFile::exists("/tmp/LCK.." + fileInfo.fileName()));
  QVERIFY(!QFile::exists("/var/lock/LCK.." + fileInfo.fileName()));
#endif

  /*
   * When port open fails, lockfiles must not exists
   */

  // Open port
  port.setPortName("akjdhuhdhkehdhieahdahiduhuhdjshiheihdu4545ffdf23dweheiuh");
  QVERIFY(port.open() != mdtAbstractPort::NoError);
  QVERIFY(!port.isOpen());

#ifdef Q_OS_UNIX
  // Check about lock file
  QVERIFY(!QFile::exists("/tmp/LCK..akjdhuhdhkehdhieahdahiduhuhdjshiheihdu4545ffdf23dweheiuh"));
  QVERIFY(!QFile::exists("/var/lock/LCK..akjdhuhdhkehdhieahdahiduhuhdjshiheihdu4545ffdf23dweheiuh"));
#endif


  /*
   * Closing by destructor: must not crash
   */
  // Open port
  port.setPortName(file.fileName());
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());
  qDebug() << "Test END";
}

void mdtPortTest::startStopTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortReadThread rdThd;
  mdtPortWriteThread wrThd;
  QTemporaryFile file;

  // Create a temporary file
  QVERIFY(file.open());

  // Setup
  port.setPortName(file.fileName());
  cfg.setReadTimeout(-1);   // Set infinite timeout
  cfg.setWriteTimeout(-1);  // Set infinite timeout
  port.setConfig(&cfg);
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  QVERIFY(port.isOpen());

  // Check read flag
  QVERIFY(rdThd.isReader());
  QVERIFY(!wrThd.isReader());

  // Assign port to the threads
  rdThd.setPort(&port);
  wrThd.setPort(&port);

  // Initial, threads stopped
  QVERIFY(!rdThd.isRunning());
  QVERIFY(!wrThd.isRunning());

  // Start/stop read thread
  rdThd.start();
  rdThd.waitReady();
  QVERIFY(rdThd.isRunning());
  rdThd.stop();
  rdThd.waitFinished();
  QVERIFY(!rdThd.isRunning());
  // Start/stop write thread
  wrThd.start();
  wrThd.waitReady();
  QVERIFY(wrThd.isRunning());
  wrThd.stop();
  wrThd.waitFinished();
  QVERIFY(!wrThd.isRunning());

  // Start threads (sequencial)
  rdThd.start();
  rdThd.waitReady();
  QVERIFY(rdThd.isRunning());
  wrThd.start();
  wrThd.waitReady();
  QVERIFY(wrThd.isRunning());
  rdThd.stop();
  rdThd.waitFinished();
  QVERIFY(wrThd.isRunning());
  QVERIFY(!rdThd.isRunning());
  wrThd.stop();
  wrThd.waitFinished();
  QVERIFY(!wrThd.isRunning());

  // Start threads (sequencial 2)
  wrThd.start();
  wrThd.waitReady();
  QVERIFY(wrThd.isRunning());
  rdThd.start();
  rdThd.waitReady();
  QVERIFY(rdThd.isRunning());
  wrThd.stop();
  wrThd.waitFinished();
  QVERIFY(!wrThd.isRunning());
  QVERIFY(rdThd.isRunning());
  rdThd.stop();
  rdThd.waitFinished();
  QVERIFY(!rdThd.isRunning());

  // Multiple start/stop - seq: rd/wr/rd/wr
  qsrand(QDateTime::currentDateTime ().toTime_t ());
  for(int i=0; i<10; i++){
    rdThd.start();
    rdThd.waitReady();
    QVERIFY(rdThd.isRunning());
    wrThd.start();
    wrThd.waitReady();
    QVERIFY(wrThd.isRunning());
    QTest::qWait((100.0*(double)qrand()) / RAND_MAX);
    rdThd.stop();
    rdThd.waitFinished();
    QVERIFY(!rdThd.isRunning());
    QVERIFY(wrThd.isRunning());
    wrThd.stop();
    wrThd.waitFinished();
    QVERIFY(!wrThd.isRunning());
  }
}

void mdtPortTest::writeRawTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortWriteThread wrThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QString filePath;
  QByteArray fileData;

  // Get data set
  QFETCH(QString, data);

  // Create a temporary file
  QVERIFY(file.open());
  filePath = file.fileName();
  file.close();

  /* Normal write */

  // Setup
  cfg.setWriteQueueSize(1);
  cfg.setFrameType(mdtFrame::FT_RAW);
  port.setPortName(file.fileName());
  port.setConfig(&cfg);
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Assign port to the threads
  wrThd.setPort(&port);

  // Start
  wrThd.start();
  wrThd.waitReady();
  QVERIFY(wrThd.isRunning());

  // Initial: empty file
  fileData = file.readAll();
  QVERIFY(fileData.size() == 0);

  // Get a frame
  port.lockMutex();
  QVERIFY(port.writeFramesPool().size() == 1);
  frame = port.writeFramesPool().dequeue();
  ///port.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->append(data);
  port.addFrameToWrite(frame);
  port.unlockMutex();
  
  
  /**
  port.lockMutex();
  port.writeFrames().enqueue(frame);
  port.unlockMutex();
  */

  // Wait some time and verify that data was written
  QTest::qWait(100);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // Close port
  wrThd.stop();
  wrThd.waitFinished();
  port.close();

  // Create a empty temporary file
  QVERIFY(file.open());
  file.remove();
  QVERIFY(!file.exists());
  file.setFileName(filePath);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == "");
  file.close();

  /* Byte per byte write */

  // Setup
  cfg.setWriteQueueSize(1);
  port.setPortName(file.fileName());
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  cfg.setBytePerByteWrite(true, 1);
  port.setConfig(&cfg);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Assign port to the threads
  wrThd.setPort(&port);

  // Start
  wrThd.start();
  wrThd.waitReady();
  QVERIFY(wrThd.isRunning());

  // Initial: empty file
  fileData = file.readAll();
  QVERIFY(fileData.size() == 0);

  // Get a frame
  port.lockMutex();
  QVERIFY(port.writeFramesPool().size() == 1);
  frame = port.writeFramesPool().dequeue();
  ///port.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->append(data);
  port.addFrameToWrite(frame);
  port.unlockMutex();
  
  
  /**
  port.lockMutex();
  port.writeFrames().enqueue(frame);
  port.unlockMutex();
  */

  // Wait some time and verify that data was written
  QTest::qWait(3*data.size()+100);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // End
  wrThd.stop();
  wrThd.waitFinished();
}

void mdtPortTest::writeRawTest_data()
{
  QString str;

  QTest::addColumn<QString>("data");

  QTest::newRow("Empty data") << "";
  QTest::newRow("1 char") << "A";
  QTest::newRow("Short len data") << "ABCD";
  QTest::newRow("Middle len data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  // Long data
  str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  str += "abcdefghijklmnopqrstuvwxyz";
  str += "0123456789";
  QTest::newRow("Long data") << str;
  // Very long data
  str = "";
  for(int i=0; i<100; i++){
    str += "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  }
  QTest::newRow("Very long data") << str;
}

void mdtPortTest::writeAsciiTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortWriteThread wrThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QString filePath;
  QByteArray fileData;

  // Get data set
  QFETCH(QString, data);

  // Create a temporary file
  QVERIFY(file.open());
  filePath = file.fileName();
  file.close();

  /* Normal write */

  // Setup
  cfg.setWriteQueueSize(1);
  cfg.setFrameType(mdtFrame::FT_ASCII);
  port.setPortName(file.fileName());
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  port.setConfig(&cfg);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Assign port to the threads
  wrThd.setPort(&port);

  // Start
  wrThd.start();
  wrThd.waitReady();
  QVERIFY(wrThd.isRunning());

  // Initial: empty file
  fileData = file.readAll();
  QVERIFY(fileData.size() == 0);

  // Get a frame
  port.lockMutex();
  QVERIFY(port.writeFramesPool().size() == 1);
  frame = port.writeFramesPool().dequeue();
  ///port.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->append(data);
  port.addFrameToWrite(frame);
  port.unlockMutex();
  
  
  /**
  port.lockMutex();
  port.writeFrames().enqueue(frame);
  port.unlockMutex();
  */

  // Wait some time and verify that data was written
  QTest::qWait(100);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // Close port
  wrThd.stop();
  wrThd.waitFinished();
  port.close();

  // Create a empty temporary file
  QVERIFY(file.open());
  file.remove();
  QVERIFY(!file.exists());
  file.setFileName(filePath);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == "");
  file.close();

  /* Byte per byte write */

  // Setup
  cfg.setWriteQueueSize(1);
  port.setPortName(file.fileName());
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  cfg.setBytePerByteWrite(true, 1);
  port.setConfig(&cfg);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Assign port to the threads
  wrThd.setPort(&port);

  // Start
  wrThd.start();
  wrThd.waitReady();
  QVERIFY(wrThd.isRunning());

  // Initial: empty file
  fileData = file.readAll();
  QVERIFY(fileData.size() == 0);

  // Get a frame
  port.lockMutex();
  QVERIFY(port.writeFramesPool().size() == 1);
  frame = port.writeFramesPool().dequeue();
  ///port.unlockMutex();
  QVERIFY(frame != 0);

  // Add some data to frame and commit
  frame->append(data);
  port.addFrameToWrite(frame);
  port.unlockMutex();
  
  
  /**
  port.lockMutex();
  port.writeFrames().enqueue(frame);
  port.unlockMutex();
  */

  // Wait some time and verify that data was written
  QTest::qWait(3*data.size()+100);
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);

  // End
  wrThd.stop();
  wrThd.waitFinished();
}

void mdtPortTest::writeAsciiTest_data()
{
  QString str;

  QTest::addColumn<QString>("data");

  QTest::newRow("Empty data") << "";
  QTest::newRow("1 char") << "A";
  QTest::newRow("Short len data") << "ABCD";
  QTest::newRow("Middle len data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  // Long data
  str = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  str += "abcdefghijklmnopqrstuvwxyz";
  str += "0123456789";
  QTest::newRow("Long data") << str;
  // Very long data
  str = "";
  for(int i=0; i<100; i++){
    str += "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  }
  QTest::newRow("Very long data") << str;
}

void mdtPortTest::readRawTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortReadThread rdThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QByteArray fileData;

  // Get data set
  QFETCH(QString, data);
  QFETCH(QStringList, refData);

  // Create a temporary file
  QVERIFY(file.open());

  // Setup
  cfg.setReadQueueSize(10);
  cfg.setReadFrameSize(10);
  cfg.setFrameType(mdtFrame::FT_RAW);
  ///QVERIFY(port.setAttributes(file.fileName()));
  port.setPortName(file.fileName());
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  port.setConfig(&cfg);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Assign port to the threads
  rdThd.setPort(&port);

  // Start
  qDebug() << "TEST: starting thd...";
  rdThd.start();
  rdThd.waitReady();
  QVERIFY(rdThd.isRunning());
  qDebug() << "TEST: thd started";

  // Write data to file and verify that data was written
  QVERIFY(file.write(data.toLocal8Bit()) == data.size());
  QVERIFY(file.flush());
  file.close();
  file.open();
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  // We have a special case for empty data test
  if((refData.size() == 1)&&(refData.at(0).size() == 0)){
    QVERIFY(port.readenFrames().size() == 0);
  }else{
    QVERIFY(port.readenFrames().size() == refData.size());
    // Verify each readen data frame
    for(int i=0; i<refData.size(); i++){
      frame = port.readenFrames().dequeue();
      QVERIFY(frame != 0);
      QVERIFY(frame->isComplete());
      // Verify readen data
      QVERIFY(*frame == refData.at(i));
      // Restore frame to pool
      port.readFramesPool().enqueue(frame);
    }
  }
  port.unlockMutex();

  // End
  qDebug() << "TEST: END, stopping thd...";
  rdThd.stop();
  rdThd.waitFinished();
  qDebug() << "TEST: END";
}

void mdtPortTest::readRawTest_data()
{
  // Note: assume that ReadFrameSize is 10
  QStringList refLst;

  QTest::addColumn<QString>("data");
  QTest::addColumn<QStringList>("refData");

  refLst.clear();
  refLst << "";
  QTest::newRow("Empty data") << "" << refLst;
  refLst.clear();
  refLst << "A";
  QTest::newRow("1 char data") << "A" << refLst;
  refLst.clear();
  refLst << "ABCD";
  QTest::newRow("1 frame short data") << "ABCD" << refLst;
  refLst.clear();
  refLst << "ABCDEFGHIJ" << "KLMNOPQRST" << "UVWXYZ";
  QTest::newRow("1 frame middle len data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << refLst;
  refLst.clear();
  refLst << "ABCDEFGHIJ" << "KLMNOPQRST" << "UVWXYZabcd" << "efghijklmn" << "opqrstuvwx" << "yz01234567" << "89";
  QTest::newRow("1 frame long data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789" << refLst;
}

void mdtPortTest::readAsciiTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortReadThread rdThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QByteArray fileData;

  // Get data set
  QFETCH(QString, data);
  QFETCH(QStringList, refData);

  // Create a temporary file
  QVERIFY(file.open());

  // Setup
  cfg.setReadQueueSize(10);
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setEndOfFrameSeq("*");
  ///QVERIFY(port.setAttributes(file.fileName()));
  port.setPortName(file.fileName());
  port.setConfig(&cfg);
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Assign port to the threads
  rdThd.setPort(&port);

  // Start
  rdThd.start();
  rdThd.waitReady();
  QVERIFY(rdThd.isRunning());

  // Write data to file and verify that data was written
  QVERIFY(file.write(data.toLocal8Bit()) == data.size());
  QVERIFY(file.flush());
  file.close();
  file.open();
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == refData.size());
  // Verify each readen data frame
  for(int i=0; i<refData.size(); i++){
    frame = port.readenFrames().dequeue();
    QVERIFY(frame != 0);
    QVERIFY(frame->isComplete());
    // Verify readen data
    QVERIFY(*frame == refData.at(i));
    // Restore frame to pool
    port.readFramesPool().enqueue(frame);
  }
  port.unlockMutex();

  // End
  rdThd.stop();
  rdThd.waitFinished();
}

void mdtPortTest::readAsciiTest_data()
{
  QString str;
  QStringList lst;

  QTest::addColumn<QString>("data");
  QTest::addColumn<QStringList>("refData");

  lst.clear();
  lst << "";
  QTest::newRow("Empty data") << "*" << lst;
  lst.clear();
  lst << "A";
  QTest::newRow("1 char data") << "A*" << lst;
  lst.clear();
  lst << "ABCD";
  QTest::newRow("1 frame short data") << "ABCD*" << lst;
  lst.clear();
  lst << "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  QTest::newRow("1 frame middle len data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ*" << lst;
  lst.clear();
  lst << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  QTest::newRow("1 frame long data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789*" << lst;
  lst.clear();
  lst << "A" << "B" << "C" << "D";
  QTest::newRow("Multi frame short data") << "A*B*C*D*" << lst;
  lst.clear();
  lst << "ABCD" << "1234" << "abcd";
  QTest::newRow("Multi frame middle len data") << "ABCD*1234*abcd*" << lst;
  lst.clear();
  lst << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << "0123456789" << "abcdefghijklmnopqrstuvwxyz";
  QTest::newRow("Multi frame long data") << "ABCDEFGHIJKLMNOPQRSTUVWXYZ*0123456789*abcdefghijklmnopqrstuvwxyz*" << lst;
}

void mdtPortTest::readInvalidDataAsciiTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortReadThread rdThd;
  mdtFrame *frame;
  QTemporaryFile file;
  QString filePath;
  QByteArray fileData;
  QByteArray data;

  // Create a temporary file
  QVERIFY(file.open());
  filePath = file.fileName();

  // Setup
  cfg.setReadFrameSize(10);
  cfg.setReadQueueSize(10);
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setEndOfFrameSeq("*");
  ///QVERIFY(port.setAttributes(file.fileName()));
  port.setPortName(file.fileName());
  port.setConfig(&cfg);
  QVERIFY(port.open() == mdtAbstractPort::NoError);

  // Assign port to the threads
  rdThd.setPort(&port);

  /*
   * Data without EOF , size < capacity : must not crash, no frame must be readen
   */

  // Write data to file and verify that data was written
  data = "01234";
  QVERIFY(file.write(data) == data.size());
  QVERIFY(file.flush());
  file.close();
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Start
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  rdThd.start();
  rdThd.waitReady();
  QVERIFY(rdThd.isRunning());

  // Wait some time and verify that no data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 0);
  port.unlockMutex();

  // Stop thread
  rdThd.stop();
  rdThd.waitFinished();
  QVERIFY(!rdThd.isRunning());
  port.close();

  /*
   * Data without EOF , size == capacity : must not crash, 1 frame must be readen
   */

  // Write data to file and verify that data was written
  QVERIFY(file.remove());
  file.setFileName(filePath);
  QVERIFY(file.open());
  data = "0123456789";
  QVERIFY(file.write(data) == data.size());
  QVERIFY(file.flush());
  file.close();
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Start
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  rdThd.start();
  rdThd.waitReady();
  QVERIFY(rdThd.isRunning());

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 1);
  frame = port.readenFrames().dequeue();
  QVERIFY(!frame->isComplete());
  // Restore frame
  port.readFramesPool().enqueue(frame);
  port.unlockMutex();

  // Stop thread
  rdThd.stop();
  rdThd.waitFinished();
  QVERIFY(!rdThd.isRunning());
  port.close();

  /*
   * Data without EOF , size > capacity : must not crash, 1 frame must be readen
   */

  // Write data to file and verify that data was written
  QVERIFY(file.remove());
  file.setFileName(filePath);
  QVERIFY(file.open());
  data = "0123456789ABCD";
  QVERIFY(file.write(data) == data.size());
  QVERIFY(file.flush());
  file.close();
  file.open();
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Start
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  rdThd.start();
  rdThd.waitReady();
  QVERIFY(rdThd.isRunning());

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 1);
  frame = port.readenFrames().dequeue();
  QVERIFY(!frame->isComplete());
  // Restore frame
  port.readFramesPool().enqueue(frame);
  port.unlockMutex();

  // End
  rdThd.stop();
  rdThd.waitFinished();
  QVERIFY(!rdThd.isRunning());
  port.close();
  
  /*
   * Data with EOF , size == capacity : must not crash, 1 frame must be readen
   */

  // Write data to file and verify that data was written
  QVERIFY(file.remove());
  file.setFileName(filePath);
  QVERIFY(file.open());
  data = "012345678*";
  QVERIFY(file.write(data) == data.size());
  QVERIFY(file.flush());
  file.close();
  QVERIFY(file.open());
  fileData = file.readAll();
  QVERIFY(fileData == data);
  file.close();

  // Start
  QVERIFY(port.setup() == mdtAbstractPort::NoError);
  rdThd.start();
  rdThd.waitReady();
  QVERIFY(rdThd.isRunning());

  // Wait some time and verify that data are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 1);
  frame = port.readenFrames().dequeue();
  QVERIFY(frame->isComplete());
  QVERIFY(*frame == "012345678");
  // Restore frame
  port.readFramesPool().enqueue(frame);
  port.unlockMutex();

  // Stop thread
  rdThd.stop();
  rdThd.waitFinished();
  QVERIFY(!rdThd.isRunning());
  port.close();

}

void mdtPortTest::emptyQueueRecoveryTest()
{
  mdtPort port;
  mdtPortConfig cfg;
  mdtPortReadThread rdThd;
  mdtFrame *frame1, *frame2, *frame3;
  QTemporaryFile file;
  QByteArray fileData;

  // Create a temporary file
  QVERIFY(file.open());
  file.close();

  // Setup
  cfg.setReadQueueSize(3);
  cfg.setFrameType(mdtFrame::FT_ASCII);
  cfg.setEndOfFrameSeq("*");
  ///QVERIFY(port.setAttributes(file.fileName()));
  port.setPortName(file.fileName());
  port.setConfig(&cfg);
  QVERIFY(port.open() == mdtAbstractPort::NoError);
  QVERIFY(port.setup() == mdtAbstractPort::NoError);

  // Assign port to the threads
  rdThd.setPort(&port);

  // Start
  rdThd.start();
  rdThd.waitReady();
  QVERIFY(rdThd.isRunning());

  // Write data
  file.open();
  QVERIFY(file.write("ABCD*EFGH*IJKL*MNOP*"));
  QVERIFY(file.flush());
  file.close();
  file.open();
  fileData = file.readAll();
  QVERIFY(fileData == "ABCD*EFGH*IJKL*MNOP*");
  file.close();

  // Wait some time, and check that 3 frames are available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 3);
  port.unlockMutex();

  // Get readen frames
  port.lockMutex();
  frame1 = port.readenFrames().dequeue();
  frame2 = port.readenFrames().dequeue();
  frame3 = port.readenFrames().dequeue();
  port.unlockMutex();
  // Check that frames are not null
  QVERIFY(frame1 != 0);
  QVERIFY(frame2 != 0);
  QVERIFY(frame3 != 0);
  // Verify data
  QVERIFY(*frame1 == "ABCD");
  QVERIFY(*frame2 == "EFGH");
  QVERIFY(*frame3 == "IJKL");

  // Restore frames to pool
  port.lockMutex();
  port.readFramesPool().enqueue(frame1);
  port.readFramesPool().enqueue(frame2);
  port.readFramesPool().enqueue(frame3);
  port.unlockMutex();

  // Wait some time, and check that 1 frame is available
  QTest::qWait(100);
  port.lockMutex();
  QVERIFY(port.readenFrames().size() == 1);
  frame1 = port.readenFrames().dequeue();
  QVERIFY(frame1 != 0);
  QVERIFY(*frame1 == "MNOP");
  port.unlockMutex();
  rdThd.stop();
  rdThd.waitFinished();
}


int main(int argc, char **argv)
{
  mdtApplication app(argc, argv);
  mdtPortTest test;

  if(!app.init()){
    return 1;
  }

  return QTest::qExec(&test, argc, argv);
}
