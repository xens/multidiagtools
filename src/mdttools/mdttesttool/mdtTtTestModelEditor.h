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
#ifndef MDT_TT_TEST_MODEL_EDITOR_H
#define MDT_TT_TEST_MODEL_EDITOR_H

///#include "mdtClEditor.h"
#include "mdtSqlForm.h"

/*! \brief Editor fot tests
 */
class mdtTtTestModelEditor : public mdtSqlForm
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtTtTestModelEditor(QWidget* parent, QSqlDatabase db);

  /*! \brief Setup tables
   */
  bool setupTables();

 private slots:

  /*! \brief Add a test node
   */
  void addTestNode();

  /*! \brief Remove test nodes
   */
  void removeTestNodes();

  /*! \brief Add a test item
   */
  void addTestItem();

  /*! \brief Edit a test item
   */
  void editTestItem();

  /*! \brief Remove a test item
   */
  void removeTestItem();

  /*! \brief Generate a 2 wire continuity test
   */
  void generateContinuityTest();

  /*! \brief Remove test node unit setup
   */
  void removeTestNodeUnitSetup();

 private:

  /*! \brief Select a test cable
   */
  QVariant selectTestCable();

  QVariant selectMeasureConnection(const QVariant& testNodeId);
  
  /*! \brief Select a test link
   */
  QVariant selectTestLink(const QVariant & cableId, const QString & bus);

  /*! \brief Setup test table
   */
  bool setupTestTable();

  /*! \brief Setup used nodes table
   */
  bool setupTestNodeTable();

  /*! \brief Setup test cable view
   */
  bool setupTestCableTable();

  /*! \brief Setup test link table
   */
  bool setupTestItemTable();

  /*! \brief Setup test node unit setup table
   */
  ///bool setupTestNodeUnitSetupTable();

  Q_DISABLE_COPY(mdtTtTestModelEditor);
};

#endif // #ifndef MDT_TT_TEST_MODEL_EDITOR_H
