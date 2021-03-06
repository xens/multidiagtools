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
#include "mdtSqlDatabaseSchemaDialog.h"
#include "mdtSqlDatabaseSchemaModel.h"
#include "mdtSqlDatabaseSchemaThread.h"
#include "mdtSqlDatabaseDialogSqlite.h"
#include "mdtSqlDatabaseSqlite.h"
#include <QPushButton>
#include <QMessageBox>

#include <QSqlDatabase>

#include <QDebug>

mdtSqlDatabaseSchemaDialog::mdtSqlDatabaseSchemaDialog(QWidget* parent)
 : QDialog(parent),
   pvModel(new mdtSqlDatabaseSchemaModel(this)),
   pvThread(new mdtSqlDatabaseSchemaThread(this))
{
  setupUi(this);
  tvObjects->setModel(pvModel);
  connect(tbSelectDatabase, &QToolButton::clicked, this, &mdtSqlDatabaseSchemaDialog::selectDatabase);
  connect(pbCreateSchema, &QPushButton::clicked, this, &mdtSqlDatabaseSchemaDialog::createSchema);
  connect(pvThread, &mdtSqlDatabaseSchemaThread::objectProgressChanged, this, &mdtSqlDatabaseSchemaDialog::updateObjectProgress);
  connect(pvThread, &mdtSqlDatabaseSchemaThread::objectStatusChanged, this, &mdtSqlDatabaseSchemaDialog::setObjectStatus);
  connect(pvThread, &QThread::finished, this, &mdtSqlDatabaseSchemaDialog::onThreadFinished);
  connect(pvThread, &mdtSqlDatabaseSchemaThread::globalProgressChanged, this, &mdtSqlDatabaseSchemaDialog::updateGlobalProgress);
  connect(pvThread, &mdtSqlDatabaseSchemaThread::globalErrorOccured, this, &mdtSqlDatabaseSchemaDialog::onThreadGlobalErrorOccured);
  connect(pvThread, &mdtSqlDatabaseSchemaThread::objectErrorOccured, this, &mdtSqlDatabaseSchemaDialog::setObjectError);
  connect(pbAbort, &QPushButton::clicked, pvThread, &mdtSqlDatabaseSchemaThread::abort);
}

void mdtSqlDatabaseSchemaDialog::setSchema(const mdtSqlDatabaseSchema & s)
{
  pvModel->setSchema(s);
}

void mdtSqlDatabaseSchemaDialog::selectDatabase()
{
  mdtSqlDatabaseDialogSqlite dialog(this);

  if(dialog.exec() != QDialog::Accepted){
    return;
  }
  pvDatabase = dialog.database().database();
  wDatabaseInfo->displayInfo(pvDatabase);
}

void mdtSqlDatabaseSchemaDialog::createSchema()
{
  // Check about open connections
  if(!assureNoOpenConnectionToDatabase()){
    return;
  }
  pvModel->clearStatusAndProgress();
  pvThread->createSchema(pvModel->schema(), pvDatabase);
}

void mdtSqlDatabaseSchemaDialog::updateObjectProgress(int objectCategory, QString objectName, int progress)
{
  auto oc = static_cast<mdtSqlDatabaseSchemaModel::ObjectCategory>(objectCategory);

  pvModel->setProgress(oc, objectName, progress);
}

void mdtSqlDatabaseSchemaDialog::setObjectStatus(int objectCategory, QString objectName, int status)
{
  auto oc = static_cast<mdtSqlDatabaseSchemaModel::ObjectCategory>(objectCategory);
  auto s = static_cast<mdtSqlDatabaseSchemaModel::Status>(status);

  pvModel->setStatus(oc, objectName, s);
}

void mdtSqlDatabaseSchemaDialog::setObjectError(int objectCategory, QString objectName, mdtError error)
{
  auto oc = static_cast<mdtSqlDatabaseSchemaModel::ObjectCategory>(objectCategory);
  QString errorText;

  errorText = error.text();
  if(!error.systemText().isEmpty()){
    errorText += "\n" + error.systemText();
  }
  pvModel->setStatus(oc, objectName, mdtSqlDatabaseSchemaModel::StatusError, errorText);
}

void mdtSqlDatabaseSchemaDialog::updateGlobalProgress(int progress)
{
  pbGlobalProgress->setValue(progress);
}

void mdtSqlDatabaseSchemaDialog::onThreadFinished()
{

}

void mdtSqlDatabaseSchemaDialog::onThreadGlobalErrorOccured(mdtError error)
{
  QMessageBox msgBox(this);

  msgBox.setText(error.text());
  msgBox.setInformativeText(error.informativeText());
  msgBox.setDetailedText(error.systemText());
  msgBox.setIcon(error.levelIcon());
  msgBox.exec();
}

bool mdtSqlDatabaseSchemaDialog::assureNoOpenConnectionToDatabase()
{
  if(mdtSqlDatabaseSqlite::getOpenConnectionReferingDatabaseCount(pvDatabase) > 0){
    QString msg;
    QMessageBox msgBox(this);
    msg = tr("Some connections are refering to selected database\n");
    msg += tr("Database:") + " '" + pvDatabase.databaseName() + "'";
    msgBox.setText(msg);
    msg = tr("It is recommended to close all connections refering this database before creating the schema on it.\n");
    msg += tr("Do you really want to create the database schema now ?");
    msgBox.setInformativeText(msg);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    if(msgBox.exec() != QMessageBox::Yes){
      return false;
    }
  }
  return true;
}
