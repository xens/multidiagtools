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
#ifndef MDT_USB_PORT_TEST_H
#define MDT_USB_PORT_TEST_H

#include "mdtTest.h"

// Pour les essais ...
#include <libusb-1.0/libusb.h>

/// \todo Add start/stop test
class mdtUsbPortTest : public mdtTest
{
 Q_OBJECT

 private:

  void fillBuffer(unsigned char *buffer, int bSize);

 private slots:

  void basicAllocFreeBenchMarks();
  void basicLockUnlockBenchmark();
  void basicLockUnlockRecursiveBenchmark();
  void standardCallBenchmark();
  void virtualCallBenchmark();

  // USB part tests
  void usbEndpointDescriptorTest();
  void deviceListTest();
  void usbPortThreadTest();

  // USBTMC part tests
  void usbtmcFrameTest();
  void usbtmcFrameBenchmark();
  void usbtmcMessageTest();
  void usbtmcMessageBenchmark();
  void usbtmcBulkTransferTest();
  void usbtmcInterruptTransferTest();
  void usbtmcControlTransferTest();
  void usbtmcTransferPoolTest();
  void usbtmcTransferPoolBenchmark();
  
  void usbtmcTransferHandlerStateMachineTest();
  
  void usbtmcPortThreadTest();
  void usbtmcPortTest();
  void usbtmcPortSetupDialogTest();

  // Make some tests with Velleman k8055 board
  ///void vellemanK8055Test();

  // Some data exchange test with Agilent DSO1000
  ///void agilentDso1000Test();

};

#endif // MDT_USB_PORT_TEST_H
