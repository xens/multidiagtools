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
#ifndef MDT_TT_BASIC_TESTER_H
#define MDT_TT_BASIC_TESTER_H

#include "ui_mdtTtBasicTester.h"
#include "mdtTtTestData.h"
#include "mdtError.h"
#include <QMainWindow>
#include <QSqlDatabase>

class mdtSqlTableWidget;
class QSqlTableModel;

/*! \brief Provide ability to run tests
 */
class mdtTtBasicTester : public QMainWindow, Ui::mdtTtBasicTester
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtBasicTester(QWidget *parent, QSqlDatabase db);

  /*! \brief Setup tables
   */
  bool setupTables();

  /*! \brief Get last error
   */
  inline mdtError lastError() const { return pvLastError; }

 private slots:

  /*! \brief Create a test
   */
  void createTest();

  /*! \brief Set test model
   */
  void setTestModel();

  /*! \brief Save test
   */
  void saveTest();

 private:

  Q_DISABLE_COPY(mdtTtBasicTester);

  /*! \brief Create new test
   */
  bool createNewTest();

  /*! \brief Save data part of Test_tbl
   */
  bool saveTestData();

  /*! \brief Display data part of Test_tbl
   */
  void displayTestData(bool getFromDatabase);

  /*! \brief Display data part of Test item
   */
  void displayTestItemData();

  /*! \brief Display last error to the user
   */
  void displayLastError();

  /*! \brief Setup TestItem_view
   */
  bool setupTestItemTable();

  /*! \brief Connect actions to slots
   */
  void connectActions();

  QSqlDatabase pvDatabase;
  mdtTtTestData pvTestData;
  QSqlTableModel *pvTestItemModel;      // Act on TestItem_tbl (R/W access)
  mdtSqlTableWidget *pvTestItemWidget;  // Display data from TestItem_view (R only access)
  mdtError pvLastError;
};

#endif  // #ifndef MDT_TT_BASIC_TESTER_H
