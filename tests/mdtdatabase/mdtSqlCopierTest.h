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
#ifndef MDT_SQL_COPIER_TEST_H
#define MDT_SQL_COPIER_TEST_H

#include "mdtTest.h"
#include "mdtSqlDatabaseManager.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QSqlDatabase>

class mdtSqlCopierTest : public mdtTest
{
 Q_OBJECT

 private slots:

  void initTestCase();
  void cleanupTestCase();

  void sqlFieldSetupDataTest();

  void fieldMappingDataTest();

  void sqlDatabaseCopierTableMappingTest();
  void sqlDatabaseCopierTableMappingModelTest();
  void sqlDatabaseCopierTableMappingDialogTest();

  void sqlDatabaseCopierMappingTest();
  void sqlDatabaseCopierMappingModelTest();
  void sqlDatabaseCopierDialogTest();

 private:

  /*
   * Create the test database
   */
  void createTestDatabase();
  // Populate database with some common data
  void populateTestDatabase();
  // Clear test database data
  void clearTestDatabaseData();

  mdtSqlDatabaseManager pvDatabaseManager;
  QFileInfo pvDbFileInfo;
};

#endif // #ifndef MDT_SQL_COPIER_TEST_H
