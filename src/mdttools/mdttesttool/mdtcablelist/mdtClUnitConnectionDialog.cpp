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
#include "mdtClUnitConnectionDialog.h"
#include "mdtClUnitConnectorData.h"
#include "mdtClUnit.h"
#include "mdtSqlSelectionDialog.h"
#include <QSqlQueryModel>
#include <QSqlError>
#include <QMessageBox>
#include <QList>
#include <QSqlRecord>
#include <QLocale>

//#include <QDebug>

mdtClUnitConnectionDialog::mdtClUnitConnectionDialog(QWidget *parent, QSqlDatabase db, mode_t mode)
 : QDialog(parent)
{
  pvDatabase = db;
  pvMode = mode;
  setupUi(this);
  connect(pbCopyFunctionEN, SIGNAL(clicked()), this, SLOT(copyFunctionEN()));
  connect(pbCopyFunctionFR, SIGNAL(clicked()), this, SLOT(copyFunctionFR()));
  connect(pbCopyFunctionDE, SIGNAL(clicked()), this, SLOT(copyFunctionDE()));
  connect(pbCopyFunctionIT, SIGNAL(clicked()), this, SLOT(copyFunctionIT()));
  connect(pbCopyContactName, SIGNAL(clicked()), this, SLOT(copyContactName()));
  hideArticleConnectionWidgets();
  connect(pbSelectUnitConnector, SIGNAL(clicked()), this, SLOT(selectUnitConnector()));
  connect(pbSelectContact, SIGNAL(clicked()), this, SLOT(selectConnection()));
  connect(pbSetNoConnector, SIGNAL(clicked()), this, SLOT(setNoConnector()));
  // In edition mode, we cannot act on unit connector
  if(pvMode != Add){
    pbSetNoConnector->setVisible(false);
    pbSelectUnitConnector->setVisible(false);
  }
  // Currently, ranges functionnality is not implemented
  gbRanges->setVisible(false);
  // Connection type
  connect(cbConnectionType, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateConnectionTypeText(const QString &)));
  populateConnectionTypeComboBox();
}

mdtClUnitConnectionDialog::~mdtClUnitConnectionDialog()
{
}

void mdtClUnitConnectionDialog::setData(const mdtClUnitConnectionData &data, const QVariant & baseArticleId)
{
  Q_ASSERT(!data.value("Unit_Id_FK").isNull());

  pvData = data;
  pvBaseArticleId = baseArticleId;
  updateDialog();
}

const mdtClUnitConnectionData &mdtClUnitConnectionDialog::data() const
{
  return pvData;
}

void mdtClUnitConnectionDialog::updateConnectionTypeText(const QString& type)
{
  QLocale locale;
  mdtClUnit unit(0, pvDatabase);
  QString fieldName;
  QString sql;
  QList<QSqlRecord> dataList;
  bool ok;

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
  // Get text
  sql = "SELECT " + fieldName + " FROM ConnectionType_tbl WHERE Code_PK = '" + type + "'";
  dataList = unit.getData(sql , &ok);
  if(!ok){
    lbConnectionTypeText->setText(tr("<Error!>"));
    return;
  }
  if(dataList.size() < 1){
    lbConnectionTypeText->setText("");
    return;
  }
  lbConnectionTypeText->setText(dataList.at(0).value(fieldName).toString());
}

void mdtClUnitConnectionDialog::copyFunctionEN()
{
  if(!leFunctionEN->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Function ENG has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leFunctionEN->setText(lbArticleFunctionEN->text());
}

void mdtClUnitConnectionDialog::copyFunctionFR()
{
  if(!leFunctionFR->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Function FRA has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leFunctionFR->setText(lbArticleFunctionFR->text());
}

void mdtClUnitConnectionDialog::copyFunctionDE()
{
  if(!leFunctionDE->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Function DEU has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leFunctionDE->setText(lbArticleFunctionDE->text());
}

void mdtClUnitConnectionDialog::copyFunctionIT()
{
  if(!leFunctionIT->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Function ITA has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leFunctionIT->setText(lbArticleFunctionIT->text());
}

void mdtClUnitConnectionDialog::selectUnitConnector()
{
  mdtSqlSelectionDialog dialog(this);
  QString sql;

  // Setup and show dialog
  sql = "SELECT Id_PK, Name FROM UnitConnector_tbl WHERE Unit_Id_FK = " + pvData.value("Unit_Id_FK").toString();
  dialog.setQuery(sql, pvDatabase, false);
  dialog.setMessage("Please select connector to use:");
  dialog.setColumnHidden("Id_PK", true);
  dialog.addColumnToSortOrder("Name", Qt::AscendingOrder);
  dialog.sort();
  dialog.addSelectionResultColumn("Id_PK");
  dialog.resize(400, 300);
  dialog.setWindowTitle(tr("Unit connector selection"));
  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  // Store selected article connector ID
  Q_ASSERT(dialog.selectionResult().size() == 1);
  pvData.setValue("UnitConnector_Id_FK", dialog.selectionResult().at(0));
  pvData.setValue("ArticleConnection_Id_FK", QVariant());
  pvData.setValue("UnitContactName", QVariant());
  pvData.articleConnectionData().setValue("ArticleContactName", QVariant());
  updateDialog();
}

void mdtClUnitConnectionDialog::setNoConnector()
{
  pvData.setValue("UnitConnector_Id_FK", QVariant());
  pvData.setValue("ArticleConnection_Id_FK", QVariant());
  pvData.setValue("UnitContactName", QVariant());
  pvData.articleConnectionData().setValue("ArticleContactName", QVariant());
  updateDialog();
}

void mdtClUnitConnectionDialog::selectConnection()
{
  mdtSqlSelectionDialog dialog(this);
  mdtClUnit unit(0, pvDatabase);
  mdtClUnitConnectorData data;
  QVariant unitConnectorId;
  QSqlQueryModel model;
  QString sql;
  bool ok;

  // Get unit connector ID
  unitConnectorId = pvData.value("UnitConnector_Id_FK");
  if(unitConnectorId.isNull()){
    if(!pvBaseArticleId.isNull()){
      setConnectionFromFreeArticleConnection();
    }
    return;
  }
  // Get unit connector data
  data = unit.getConnectorData(unitConnectorId, &ok, false, true, true);
  if(!ok){
    lbUnitContactName->setText("<Error!>");
    return;
  }
  // Check on what connector is based and let user select contacts
  if(!data.value("ArticleConnector_Id_FK").isNull()){
    setConnectionFromArticleConnectorConnection(data.value("ArticleConnector_Id_FK"));
  }else if(!data.value("Connector_Id_FK").isNull()){
    setConnectionFromConnectorContact(data.value("Connector_Id_FK"));
  }

}


void mdtClUnitConnectionDialog::copyContactName()
{
  if(!leUnitContactName->text().isEmpty()){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Contact name has allready data."));
    msgBox.setInformativeText(tr("Do you want to overwrite ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  leUnitContactName->setText(lbContactName->text());
}

void mdtClUnitConnectionDialog::accept()
{
  // Checks
  /**
  if((leUnitConnectorName->text().isEmpty())&&(leUnitContactName->text().isEmpty())){
    QMessageBox msgBox(this);
    msgBox.setText(tr("Connector name and contact name are both empty."));
    msgBox.setInformativeText(tr("Do you want to continue ?"));
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return;
    }
  }
  */
  // Store data and accept
  updateData();
  QDialog::accept();
}

void mdtClUnitConnectionDialog::reject()
{
  pvData.clear();
  QDialog::reject();
}

void mdtClUnitConnectionDialog::populateConnectionTypeComboBox()
{
  mdtClUnit unit(0, pvDatabase);
  QList<QSqlRecord> dataList;
  bool ok;
  int i;

  cbConnectionType->clear();
  // Get available codes
  dataList = unit.getData("SELECT Code_PK FROM ConnectionType_tbl", &ok);
  if(!ok){
    cbConnectionType->addItem(tr("<Error!>"));
    lbConnectionTypeText->setText(tr("<Error!>"));
    return;
  }
  // Populate combo box
  for(i = 0; i < dataList.size(); ++i){
    cbConnectionType->addItem(dataList.at(i).value("Code_PK").toString());
  }
}

void mdtClUnitConnectionDialog::setCurrentConnectionType(const QString& type)
{
  int idx;

  for(idx = 0; idx < cbConnectionType->count(); ++idx){
    if(cbConnectionType->itemText(idx) == type){
      cbConnectionType->setCurrentIndex(idx);
      return;
    }
  }
  // type not found
  cbConnectionType->setCurrentIndex(-1);
}

void mdtClUnitConnectionDialog::setConnectionFromFreeArticleConnection()
{
  mdtSqlSelectionDialog selectionDialog;
  mdtClUnit unit(this, pvDatabase);
  QString sql;
  QVariant unitId;

  unitId = pvData.value("Unit_Id_FK");
  if((pvBaseArticleId.isNull())||(unitId.isNull())){
    return;
  }
  // Setup model to show available article connections
  sql = unit.sqlForFreeArticleConnectionSelection(pvBaseArticleId, unitId);
  // Setup and show dialog
  selectionDialog.setMessage("Please select article connection to use:");
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.setHeaderData("ArticleContactName", tr("Contact"));
  selectionDialog.setHeaderData("IoType", tr("I/O type"));
  selectionDialog.setHeaderData("FunctionEN", tr("Function\n(English)"));
  selectionDialog.setHeaderData("FunctionFR", tr("Function\n(French)"));
  selectionDialog.setHeaderData("FunctionDE", tr("Function\n(German)"));
  selectionDialog.setHeaderData("FunctionIT", tr("Function\n(Italian)"));
  selectionDialog.addColumnToSortOrder("ArticleContactName", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.addSelectionResultColumn("ArticleContactName");
  selectionDialog.resize(700, 300);
  selectionDialog.setWindowTitle(tr("Article connection selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 2);
  // Store result
  pvData.setValue("ArticleConnection_Id_FK", selectionDialog.selectionResult().at(0));
  pvData.setValue("UnitContactName", selectionDialog.selectionResult().at(1));
  pvData.articleConnectionData().setValue("ArticleContactName", selectionDialog.selectionResult().at(1));
  updateDialog();
}

void mdtClUnitConnectionDialog::setConnectionFromArticleConnectorConnection(const QVariant & articleConnectorId)
{
  mdtSqlSelectionDialog selectionDialog;
  mdtClUnit unit(this, pvDatabase);
  QString sql;
  QVariant unitId;

  unitId = pvData.value("Unit_Id_FK");
  if((articleConnectorId.isNull())||(unitId.isNull())){
    return;
  }
  // Setup model to show available article connections
  sql = unit.sqlForArticleConnectionLinkedToArticleConnectorSelection(articleConnectorId, unitId);
  // Setup and show dialog
  selectionDialog.setMessage("Please select article connection to use:");
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Article_Id_FK", true);
  selectionDialog.setColumnHidden("ArticleConnector_Id_FK", true);
  selectionDialog.setHeaderData("ArticleContactName", tr("Contact"));
  selectionDialog.setHeaderData("IoType", tr("I/O type"));
  selectionDialog.setHeaderData("FunctionEN", tr("Function\n(English)"));
  selectionDialog.setHeaderData("FunctionFR", tr("Function\n(French)"));
  selectionDialog.setHeaderData("FunctionDE", tr("Function\n(German)"));
  selectionDialog.setHeaderData("FunctionIT", tr("Function\n(Italian)"));
  selectionDialog.addColumnToSortOrder("ArticleContactName", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.addSelectionResultColumn("ArticleContactName");
  selectionDialog.resize(700, 300);
  selectionDialog.setWindowTitle(tr("Article connection selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 2);
  // Store result
  pvData.setValue("ArticleConnection_Id_FK", selectionDialog.selectionResult().at(0));
  pvData.setValue("UnitContactName", selectionDialog.selectionResult().at(1));
  pvData.articleConnectionData().setValue("ArticleContactName", selectionDialog.selectionResult().at(1));
  updateDialog();
}

void mdtClUnitConnectionDialog::setConnectionFromConnectorContact(const QVariant & connectorId)
{
  mdtSqlSelectionDialog selectionDialog;
  mdtClUnit unit(this, pvDatabase);
  QString sql;

  if(connectorId.isNull()){
    return;
  }
  // Setup model to show available connector contacts
  sql = unit.sqlForConnectorContactSelection(connectorId);
  // Setup and show dialog
  selectionDialog.setMessage("Please select contact to use:");
  selectionDialog.setQuery(sql, pvDatabase, false);
  selectionDialog.setColumnHidden("Id_PK", true);
  selectionDialog.setColumnHidden("Connector_Id_FK", true);
  selectionDialog.setHeaderData("Name", tr("Contact"));
  selectionDialog.addColumnToSortOrder("Name", Qt::AscendingOrder);
  selectionDialog.sort();
  selectionDialog.addSelectionResultColumn("Id_PK");
  selectionDialog.addSelectionResultColumn("Name");
  selectionDialog.resize(400, 300);
  selectionDialog.setWindowTitle(tr("Contact selection"));
  if(selectionDialog.exec() != QDialog::Accepted){
    return;
  }
  Q_ASSERT(selectionDialog.selectionResult().size() == 2);
  // Store result
  pvData.setValue("UnitContactName", selectionDialog.selectionResult().at(1));
  updateDialog();
}


void mdtClUnitConnectionDialog::updateConnectorData()
{
  mdtClUnit unit(0, pvDatabase);
  mdtClUnitConnectorData data;
  QVariant unitConnectorId;
  bool ok;

  // Get unit connector data
  unitConnectorId = pvData.value("UnitConnector_Id_FK");
  if(unitConnectorId.isNull()){
    lbUnitConnectorName->setText("");
    lbArticleConnectorNameLabel->setVisible(false);
    lbArticleConnectorName->setVisible(false);
    if((pvBaseArticleId.isNull())||(pvMode != Add)){
      pbSelectContact->setVisible(false);
    }else{
      pbSelectContact->setVisible(true);
    }
    return;
  }
  data = unit.getConnectorData(unitConnectorId, &ok, false, true, true);
  if(!ok){
    lbUnitConnectorName->setText("<Error!>");
    return;
  }
  // Update unit connector part
  lbUnitConnectorName->setText(data.value("Name").toString());
  if(pvMode == Add){
    pbSelectContact->setVisible(true);
  }else{
    pbSelectContact->setVisible(false);
  }
  // Update article connector part
  if(data.value("ArticleConnector_Id_FK").isNull()){
    lbArticleConnectorNameLabel->setVisible(false);
    lbArticleConnectorName->setVisible(false);
  }else{
    lbArticleConnectorNameLabel->setVisible(true);
    lbArticleConnectorName->setVisible(true);
    lbArticleConnectorName->setText(data.articleConnectorData().value("Name").toString());
  }
}

void mdtClUnitConnectionDialog::updateDialog()
{
  // Connector part
  updateConnectorData();
  // Article connection data part
  if(pvData.articleConnectionData().value("ArticleContactName").isNull()){
    lbContactName->setVisible(false);
    pbCopyContactName->setVisible(false);
  }else{
    lbContactName->setVisible(true);
    pbCopyContactName->setVisible(true);
    lbContactName->setText(pvData.articleConnectionData().value("ArticleContactName").toString());
  }
  if(pvData.articleConnectionData().value("IoType").isNull()){
    lbArticleIoType->setVisible(false);
    lbArticleIoTypeLabel->setVisible(false);
  }else{
    lbArticleIoType->setVisible(true);
    lbArticleIoTypeLabel->setVisible(true);
    lbArticleIoType->setText(pvData.articleConnectionData().value("IoType").toString());
  }
  if(pvData.articleConnectionData().value("FunctionEN").isNull()){
    lbArticleFunctionEN->setVisible(false);
    pbCopyFunctionEN->setVisible(false);
  }else{
    lbArticleFunctionEN->setVisible(true);
    pbCopyFunctionEN->setVisible(true);
    lbArticleFunctionEN->setText(pvData.articleConnectionData().value("FunctionEN").toString());
  }
  if(pvData.articleConnectionData().value("FunctionFR").isNull()){
    lbArticleFunctionFR->setVisible(false);
    pbCopyFunctionFR->setVisible(false);
  }else{
    lbArticleFunctionFR->setVisible(true);
    pbCopyFunctionFR->setVisible(true);
    lbArticleFunctionFR->setText(pvData.articleConnectionData().value("FunctionFR").toString());
  }
  if(pvData.articleConnectionData().value("FunctionDE").isNull()){
    lbArticleFunctionDE->setVisible(false);
    pbCopyFunctionDE->setVisible(false);
  }else{
    lbArticleFunctionDE->setVisible(true);
    pbCopyFunctionDE->setVisible(true);
    lbArticleFunctionDE->setText(pvData.articleConnectionData().value("FunctionDE").toString());
  }
  if(pvData.articleConnectionData().value("FunctionIT").isNull()){
    lbArticleFunctionIT->setVisible(false);
    pbCopyFunctionIT->setVisible(false);
  }else{
    lbArticleFunctionIT->setVisible(true);
    pbCopyFunctionIT->setVisible(true);
    lbArticleFunctionIT->setText(pvData.articleConnectionData().value("FunctionIT").toString());
  }
  // Unit connection pvData part
  setCurrentConnectionType(pvData.value("ConnectionType_Code_FK").toString());
  leSchemaPage->setText(pvData.value("SchemaPage").toString());
  leFunctionEN->setText(pvData.value("FunctionEN").toString());
  leFunctionFR->setText(pvData.value("FunctionFR").toString());
  leFunctionDE->setText(pvData.value("FunctionDE").toString());
  leFunctionIT->setText(pvData.value("FunctionIT").toString());
  leSignalName->setText(pvData.value("SignalName").toString());
  sbSwAddress->setValue(pvData.value("SwAddress").toInt());
  leUnitContactName->setText(pvData.value("UnitContactName").toString());
}

void mdtClUnitConnectionDialog::updateData()
{
  pvData.setValue("ConnectionType_Code_FK", cbConnectionType->currentText());
  if(leSchemaPage->text().isEmpty()){
    pvData.setValue("SchemaPage", QVariant());
  }else{
    pvData.setValue("SchemaPage", leSchemaPage->text());
  }
  if(leFunctionEN->text().isEmpty()){
    pvData.setValue("FunctionEN", QVariant());
  }else{
    pvData.setValue("FunctionEN", leFunctionEN->text());
  }
  if(leFunctionFR->text().isEmpty()){
    pvData.setValue("FunctionFR", QVariant());
  }else{
    pvData.setValue("FunctionFR", leFunctionFR->text());
  }
  if(leFunctionDE->text().isEmpty()){
    pvData.setValue("FunctionDE", QVariant());
  }else{
    pvData.setValue("FunctionDE", leFunctionDE->text());
  }
  if(leFunctionIT->text().isEmpty()){
    pvData.setValue("FunctionIT", QVariant());
  }else{
    pvData.setValue("FunctionIT", leFunctionIT->text());
  }
  if(leSignalName->text().isEmpty()){
    pvData.setValue("SignalName", QVariant());
  }else{
    pvData.setValue("SignalName", leSignalName->text());
  }
  if(sbSwAddress->value() == 0){
    pvData.setValue("SwAddress", QVariant());
  }else{
    pvData.setValue("SwAddress", sbSwAddress->value());
  }
  if(leUnitContactName->text().isEmpty()){
    pvData.setValue("UnitContactName", QVariant());
  }else{
    pvData.setValue("UnitContactName", leUnitContactName->text());
  }
}


void mdtClUnitConnectionDialog::hideArticleConnectionWidgets()
{
  lbArticleIoTypeLabel->setVisible(false);
  lbArticleIoType->setVisible(false);
  lbArticleFunctionEN->setVisible(false);
  pbCopyFunctionEN->setVisible(false);
  lbArticleFunctionFR->setVisible(false);
  pbCopyFunctionFR->setVisible(false);
  lbArticleFunctionDE->setVisible(false);
  pbCopyFunctionDE->setVisible(false);
  lbArticleFunctionIT->setVisible(false);
  pbCopyFunctionIT->setVisible(false);
  lbArticleConnectorName->setVisible(false);
  lbArticleConnectorNameLabel->setVisible(false);
  pbCopyContactName->setVisible(false);
}

void mdtClUnitConnectionDialog::showArticleConnectionWidgets()
{
  lbArticleIoTypeLabel->setVisible(true);
  lbArticleIoType->setVisible(true);
  lbArticleFunctionEN->setVisible(true);
  pbCopyFunctionEN->setVisible(true);
  lbArticleFunctionFR->setVisible(true);
  pbCopyFunctionFR->setVisible(true);
  lbArticleFunctionDE->setVisible(true);
  pbCopyFunctionDE->setVisible(true);
  lbArticleFunctionIT->setVisible(true);
  pbCopyFunctionIT->setVisible(true);
  lbArticleConnectorName->setVisible(true);
  lbArticleConnectorNameLabel->setVisible(true);
  pbCopyContactName->setVisible(true);
}
