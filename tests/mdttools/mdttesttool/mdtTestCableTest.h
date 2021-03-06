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
#ifndef MDT_TEST_CABLE_TEST_H
#define MDT_TEST_CABLE_TEST_H

#include "mdtTest.h"
#include "mdtSqlDatabaseManager.h"
#include "mdtTtTestNodeRouteData.h"
#include <QString>
#include <QVariant>

class mdtTestCableTest : public mdtTest
{
 Q_OBJECT

 private slots:

  // Will create database schema (see createDatabaseSchema() )
  void initTestCase();
  void cleanupTestCase();

  void sandbox();

 private:

  // Create test database schema - Will FAIL on problem
  void createDatabaseSchema();

  mdtSqlDatabaseManager pvDatabaseManager;
};

#endif // #ifndef MDT_TEST_CABLE_TEST_H


