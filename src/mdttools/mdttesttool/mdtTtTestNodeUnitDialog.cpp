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
#include "mdtTtTestNodeUnitDialog.h"
#include "mdtTtTestNodeUnit.h"
#include "mdtSqlRecord.h"
#include "mdtClUnit.h"
#include "mdtClUnitConnectorData.h"
#include "mdtClUnitConnectionData.h"
#include "mdtSqlSelectionDialog.h"
#include <QString>
#include <QStringList>
#include <QLocale>
#include <QList>
#include <QVariant>
#include <QMessageBox>
#include <QModelIndex>

mdtTtTestNodeUnitDialog::mdtTtTestNodeUnitDialog(QWidget *parent, QSqlDatabase db, mdtTtTestNodeUnitDialog::mode_t mode)
 : QDialog(parent)
{
  pvDatabase = db;
  // Setup UI
  setupUi(this);
  setupTypeComboBox();
  if(mode == Add){
    pbSelectUnit->setVisible(true);
    connect(pbSelectUnit, SIGNAL(clicked()), this, SLOT(selectBaseUnit()));
  }else{
    pbSelectUnit->setVisible(false);
  }
  updateDialog();
}

void mdtTtTestNodeUnitDialog::setData(const mdtTtTestNodeUnitData & data) 
{
  pvData = data;
  updateDialog();
}

mdtTtTestNodeUnitData mdtTtTestNodeUnitDialog::data()
{
  return pvData;
}

void mdtTtTestNodeUnitDialog::selectBaseUnit() 
{
  mdtTtTestNodeUnit tnu(0, pvDatabase);
  mdtSqlSelectionDialog selectionDialog;
  QString sql;
  mdtSqlRecord unitData;
  bool ok;
  QString testNodeId;

  if(pvData.value("TestNode_Id_FK").isNull()){
    return;
  }
  // Setup SQL
  testNodeId = pvData.value("TestNode_Id_FK").toString();
  sql = "SELECT * FROM Unit_view ";
  sql += " WHERE VehicleType_Id_PK = " + testNodeId;
  sql += " AND Unit_Id_PK NOT IN (";
  sql += "  SELECT Unit_Id_FK_PK FROM TestNodeUnit_view WHERE TestNode_Id_FK = " + testNodeId;
  sql += ")";
  // Setup and show dialog
  selectionDialog.setMessage("Please select unit:");
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("VehicleType_Id_PK", true);
  selectionDialog.setColumnHidden("Unit_Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_PK", true);
  selectionDialog.setColumnHidden("Type", true);
  selectionDialog.setColumnHidden("SubType", true);
  selectionDialog.setColumnHidden("SeriesNumber", true);
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema position"));
  selectionDialog.setHeaderData("ArticleCode", tr("Article code"));
  selectionDialog.setHeaderData("DesignationEN", tr("Designation\n(English)"));
  selectionDialog.setHeaderData("DesignationFR", tr("Designation\n(French)"));
  selectionDialog.setHeaderData("DesignationDE", tr("Designation\n(German)"));
  selectionDialog.setHeaderData("DesignationIT", tr("Designation\n(Italian)"));
  selectionDialog.setHeaderData("SchemaPosition", tr("Schema position"));
  selectionDialog.addColumnToSortOrder("SchemaPosition", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Unit_Id_PK");
  selectionDialog.resize(700, 400);
  selectionDialog.setWindowTitle(tr("Unit selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 1);
  // Store result
  unitData = tnu.getUnitData(selectionDialog.selectionResult().at(0), &ok);
  /// \todo Message on error

  pvData.setUnitData(unitData);
  /// \todo Clear connection data
  updateDialog();
}

void mdtTtTestNodeUnitDialog::accept() 
{
  QString msg;
  QStringList missingFields;
  int i;

  storeType();
  if(sbIoPosition->value() < 0){
    pvData.setValue("IoPosition", QVariant());
  }else{
    pvData.setValue("IoPosition", sbIoPosition->value());
  }
  // Check required data
  if(pvData.value("Unit_Id_FK_PK").isNull()){
    missingFields << " - Unit";
  }
  if(pvData.value("Type_Code_FK").isNull()){
    missingFields << " - Type";
  }
  /**
  if(pvData.value("IoPosition").isNull()){
    missingFields << " - I/O position";
  }
  */
  if(!missingFields.isEmpty()){
    msg = tr("Some data are missing. Please check:") + "\n";
    for(i = 0; i < missingFields.size(); ++i){
      msg += missingFields.at(i) + "\n";
    }
    QMessageBox msgBox(this);
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setWindowTitle(tr("Test link data missing"));
    msgBox.exec();
    return;
  }
  QDialog::accept();
}

void mdtTtTestNodeUnitDialog::reject() 
{
  pvData.clearValues();
  QDialog::reject();
}

void mdtTtTestNodeUnitDialog::updateDialog()
{
  mdtSqlRecord unitData;

  // Get data
  unitData = pvData.unitData();
  // Update UI
  if(pvData.value("IoPosition").isNull()){
    sbIoPosition->setValue(-1);
  }else{
    sbIoPosition->setValue(pvData.value("IoPosition").toInt());
  }
  lbSchemaPosition->setText(unitData.value("SchemaPosition").toString());
  lbAlias->setText(unitData.value("Alias").toString());
  displayType();
}

void mdtTtTestNodeUnitDialog::displayType()
{
  int row;
  QModelIndex index;
  QVariant data;

  for(row = 0; row < pvTypeModel.rowCount(); ++row){
    index = pvTypeModel.index(row, 0);
    data = pvTypeModel.data(index);
    if(data == pvData.value("Type_Code_FK")){
      cbType->setCurrentIndex(row);
      return;
    }
  }
  cbType->setCurrentIndex(-1);
}

void mdtTtTestNodeUnitDialog::storeType()
{
  int row;
  QModelIndex index;

  row = cbType->currentIndex();
  if(row < 0){
    pvData.setValue("Type_Code_FK", QVariant());
    return;
  }
  index = pvTypeModel.index(row, 0);
  if(!index.isValid()){
    pvData.setValue("Type_Code_FK", QVariant());
    return;
  }
  pvData.setValue("Type_Code_FK", pvTypeModel.data(index));
}

void mdtTtTestNodeUnitDialog::setupTypeComboBox() 
{
  QLocale locale;
  QString fieldName;
  QString sql;

  // Select field regarding language
  switch(locale.language()){
    case QLocale::French:
      fieldName = "NameFR";
      break;
    case QLocale::German:
      fieldName = "NameDE";
      break;
    case QLocale::Italian:
      fieldName = "NameIT";
      break;
    default:
      fieldName = "NameEN";
  }
  // Setup type model and combobox
  sql = "SELECT Code_PK, " + fieldName + " FROM TestNodeUnitType_tbl";
  pvTypeModel.setQuery(sql, pvDatabase);
  cbType->setModel(&pvTypeModel);
  cbType->setModelColumn(1);
}
