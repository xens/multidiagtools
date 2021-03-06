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
#ifndef MDT_ALGORITHMS_TEST_H
#define MDT_ALGORITHMS_TEST_H

#include "mdtTest.h"

class mdtAlgorithmsTest : public mdtTest
{
 Q_OBJECT

 private slots:

  // Numeric range test
  void numericRangeDoubleTest();
  void numericRangeDoubleBenchmark();
  void numericRangeDoubleSearchBenchmark();

  // Check natural sort
  void naturalCompareLessThanTest();
  void naturalCompareLessThanTest_data();
  void naturalSortTest();
  void naturalSortTest_data();

  // Test hex string <-> byte array conversions
  void hexStringByteArrayTest();

  // Unprotected string test
  void unprotectedStringTest();

  // Split string test
  void splitStringTest();

  void longestLineInStringTest();

  // String generation test
  void generateStringTest();
  void generateStringBenchmark();
};

#endif  // #ifndef MDT_ALGORITHMS_TEST_H
