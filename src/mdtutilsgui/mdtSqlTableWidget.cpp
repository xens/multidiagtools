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
#include "mdtSqlTableWidget.h"
#include <QTableView>
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSqlTableModel>
#include <QModelIndex>
#include <QItemSelectionModel>
#include <QSqlError>
#include <QMessageBox>
#include <QPushButton>
#include <QPushButton>
#include <QList>
#include <QKeyEvent>
#include <QObject>
#include <QWidget>

#include <QDebug>

/*
 * mdtSqlTableWidgetKeyEventFilter implementation
 */

mdtSqlTableWidgetKeyEventFilter::mdtSqlTableWidgetKeyEventFilter(QObject *parent)
 : QObject(parent)
{
}

bool mdtSqlTableWidgetKeyEventFilter::eventFilter(QObject *obj, QEvent *event)
{
  Q_ASSERT(obj != 0);
  Q_ASSERT(event != 0);

  if(event->type() == QEvent::KeyPress){
    QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
    // emit signal if we have a known key
    switch(keyEvent->key()){
      case Qt::Key_Enter:
      case Qt::Key_Return:
      case Qt::Key_Delete:
      case Qt::Key_Down:
        emit knownKeyPressed(keyEvent->key());
        return true;
      default:
        return false;
    }
  }else{
    return QObject::eventFilter(obj, event);
  }
}

/*
 * mdtSqlTableWidgetItemDelegate implementation
 */

mdtSqlTableWidgetItemDelegate::mdtSqlTableWidgetItemDelegate(QObject *parent)
 : QStyledItemDelegate(parent)
{
}

QWidget *mdtSqlTableWidgetItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  emit dataEditionBegins();
  return QStyledItemDelegate::createEditor(parent, option, index);
}

/*
 * mdtSqlTableWidget implementation
 */

mdtSqlTableWidget::mdtSqlTableWidget(QWidget *parent)
 : mdtAbstractSqlWidget(parent)
{
  pvTableView = new QTableView;
  pvTableView->setSelectionBehavior(QAbstractItemView::SelectItems);
  connect(this, SIGNAL(modelSelected()), this, SLOT(onModelSelected()));
  QVBoxLayout *layout = new QVBoxLayout;
  // Install event filter on table view to catch some key events
  mdtSqlTableWidgetKeyEventFilter *keyEventFilter = new mdtSqlTableWidgetKeyEventFilter(this);
  pvTableView->installEventFilter(keyEventFilter);
  connect(keyEventFilter, SIGNAL(knownKeyPressed(int)), this, SLOT(onTableViewKnownKeyPressed(int)));
  // Set our custom item delegate
  mdtSqlTableWidgetItemDelegate *delegate = new mdtSqlTableWidgetItemDelegate(this);
  pvTableView->setItemDelegate(delegate);
  connect(delegate, SIGNAL(dataEditionBegins()), this, SLOT(onDataEditionBegins()));

  layout->addWidget(pvTableView);
  setLayout(layout);

  pvNavigationLayout = 0;
  pbNavToFirst = 0;
  pbNavToPrevious = 0;
  pbNavToNext = 0;
  pbNavToLast = 0;
  pbInsert = 0;
  pbSubmit = 0;
  pbRevert = 0;
  pbRemove = 0;
  pvDelegateIsEditingData = false;
  pvDefaultColumnToSelect = 0;

  setEditionEnabled(false);
}

mdtSqlTableWidget::~mdtSqlTableWidget()
{
}

int mdtSqlTableWidget::currentRow() const
{
  return pvTableView->currentIndex().row();
}

void mdtSqlTableWidget::setEditionEnabled(bool enable)
{
  if(enable){
    ///pvTableView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    pvTableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);
  }else{
    pvTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  }
}

void mdtSqlTableWidget::enableLocalEdition()
{
  Q_ASSERT(layout() != 0);

  QBoxLayout *boxLayout;

  // Create actions
  pbInsert = new QPushButton("New");
  pbSubmit = new QPushButton("Save");
  pbRevert = new QPushButton("Cancel");
  pbRemove = new QPushButton("Delete");
  // As default, functions are disabled
  pbInsert->setEnabled(false);
  pbSubmit->setEnabled(false);
  pbRevert->setEnabled(false);
  pbRemove->setEnabled(false);
  // Connect actions enable/disable
  connect(this, SIGNAL(insertEnabledStateChanged(bool)), pbInsert, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(submitEnabledStateChanged(bool)), pbSubmit, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(revertEnabledStateChanged(bool)), pbRevert, SLOT(setEnabled(bool)));
  connect(this, SIGNAL(removeEnabledStateChanged(bool)), pbRemove, SLOT(setEnabled(bool)));
  // Connect actions triggers
  connect(pbInsert, SIGNAL(clicked()), this, SLOT(insert()));
  connect(pbSubmit, SIGNAL(clicked()), this, SLOT(submit()));
  connect(pbRevert, SIGNAL(clicked()), this, SLOT(revert()));
  connect(pbRemove, SIGNAL(clicked()), this, SLOT(remove()));
  // Add to layout
  if(pvNavigationLayout == 0){
    pvNavigationLayout = new QHBoxLayout;
    ///pvNavigationLayout->addStretch();
    boxLayout = dynamic_cast<QBoxLayout*>(layout());
    Q_ASSERT(boxLayout != 0);
    boxLayout->addLayout(pvNavigationLayout);
  }
  Q_ASSERT(pvNavigationLayout != 0);
  pvNavigationLayout->addWidget(pbInsert);
  pvNavigationLayout->addWidget(pbSubmit);
  pvNavigationLayout->addWidget(pbRevert);
  pvNavigationLayout->addWidget(pbRemove);
  pvNavigationLayout->addStretch();
  setEditionEnabled(true);
}

void mdtSqlTableWidget::addWidgetToLocalBar(QWidget *widget)
{
  Q_ASSERT(widget != 0);
  Q_ASSERT(layout() != 0);

  QBoxLayout *boxLayout;

  // Add to layout
  if(pvNavigationLayout == 0){
    pvNavigationLayout = new QHBoxLayout;
    boxLayout = dynamic_cast<QBoxLayout*>(layout());
    Q_ASSERT(boxLayout != 0);
    boxLayout->addLayout(pvNavigationLayout);
  }
  Q_ASSERT(pvNavigationLayout != 0);
  pvNavigationLayout->addWidget(widget);
}

void mdtSqlTableWidget::addStretchToLocalBar()
{
  Q_ASSERT(layout() != 0);

  QBoxLayout *boxLayout;

  // Add to layout
  if(pvNavigationLayout == 0){
    pvNavigationLayout = new QHBoxLayout;
    boxLayout = dynamic_cast<QBoxLayout*>(layout());
    Q_ASSERT(boxLayout != 0);
    boxLayout->addLayout(pvNavigationLayout);
  }
  Q_ASSERT(pvNavigationLayout != 0);
  pvNavigationLayout->addStretch();
}

QItemSelectionModel *mdtSqlTableWidget::selectionModel()
{
  return pvTableView->selectionModel();
}

QTableView *mdtSqlTableWidget::tableView()
{
  return pvTableView;
}

void mdtSqlTableWidget::setHeaderData(const QString &fieldName, const QString &data)
{
  Q_ASSERT(model() != 0);

  int column;

  column = model()->record().indexOf(fieldName);
  if(column < 0){
    return;
  }
  model()->setHeaderData(column, Qt::Horizontal, data);
}

void mdtSqlTableWidget::setColumnHidden(int column, bool hide)
{
  pvTableView->setColumnHidden(column, hide);
  if(model() != 0){
    setDefaultColumnToSelect(firstVisibleColumnIndex());
  }
}

void mdtSqlTableWidget::setColumnHidden(const QString &fieldName, bool hide)
{
  Q_ASSERT(model() != 0);

  int column;

  column = model()->record().indexOf(fieldName);
  if(column < 0){
    return;
  }
  setColumnHidden(column, hide);
}

void mdtSqlTableWidget::setDefaultColumnToSelect(int column)
{
  Q_ASSERT(model() != 0);

  if((column < 0)||(column >= model()->columnCount())){
    pvDefaultColumnToSelect = 0;
  }else{
    pvDefaultColumnToSelect = column;
  }
}

void mdtSqlTableWidget::setDefaultColumnToSelect(const QString &fieldName)
{
  Q_ASSERT(model() != 0);

  setDefaultColumnToSelect(model()->fieldIndex(fieldName));
}

QList<QModelIndexList> mdtSqlTableWidget::indexListOfSelectedRowsByRowsList(const QList<int> &columnList)
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(selectionModel() != 0);

  QModelIndexList indexes;
  QList<QModelIndexList> result;
  QModelIndexList rowResult;
  QModelIndex index;
  QList<int> selectedRows;
  int i, j;
  int row;
  int column;

  // Get currently selected indexes
  indexes = selectionModel()->selectedIndexes();
  // Build a list of selected rows
  for(i = 0; i < indexes.size(); ++i){
    row = indexes.at(i).row();
    Q_ASSERT(row >= 0);
    if(!selectedRows.contains(row)){
      selectedRows.append(row);
    }
  }
  qSort(selectedRows.begin(), selectedRows.end());
  // Build list with only interresting columns
  for(i = 0; i < selectedRows.size(); ++i){
    row = selectedRows.at(i);
    rowResult.clear();
    for(j = 0; j < columnList.size(); ++j){
      column = columnList.at(j);
      index = model()->index(row, column);
      Q_ASSERT(index.isValid());
      rowResult.append(index);
    }
    result.append(rowResult);
  }

  return result;
}

QList<QModelIndexList> mdtSqlTableWidget::indexListOfSelectedRowsByRowsList(const QStringList &fieldList)
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(selectionModel() != 0);

  QList<int> columns;
  int i;

  for(i = 0; i < fieldList.size(); ++i){
    columns.append(model()->fieldIndex(fieldList.at(i)));
  }

  return indexListOfSelectedRowsByRowsList(columns);
}

QModelIndexList mdtSqlTableWidget::indexListOfSelectedRows(const QList<int> &columnList)
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(selectionModel() != 0);

  QModelIndexList indexes;
  QModelIndexList result;
  QModelIndex index;
  QList<int> selectedRows;
  int i, j;
  int row;
  int column;

  // Get currently selected indexes
  indexes = selectionModel()->selectedIndexes();
  // Build a list of selected rows
  for(i = 0; i < indexes.size(); ++i){
    row = indexes.at(i).row();
    Q_ASSERT(row >= 0);
    if(!selectedRows.contains(row)){
      selectedRows.append(row);
    }
  }
  qSort(selectedRows.begin(), selectedRows.end());
  // Build list with only interresting columns
  for(i = 0; i < selectedRows.size(); ++i){
    row = selectedRows.at(i);
    for(j = 0; j < columnList.size(); ++j){
      column = columnList.at(j);
      index = model()->index(row, column);
      Q_ASSERT(index.isValid());
      result.append(index);
    }
  }

  return result;
}

QModelIndexList mdtSqlTableWidget::indexListOfSelectedRows(const QStringList &fieldList)
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(selectionModel() != 0);

  QList<int> columns;
  int i;

  for(i = 0; i < fieldList.size(); ++i){
    columns.append(model()->fieldIndex(fieldList.at(i)));
  }

  return indexListOfSelectedRows(columns);
}

QModelIndexList mdtSqlTableWidget::indexListOfSelectedRows(const QString &field)
{
  Q_ASSERT(model() != 0);
  Q_ASSERT(selectionModel() != 0);

  return indexListOfSelectedRows(QStringList(field));
}

void mdtSqlTableWidget::setCurrentIndex(int row)
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  QModelIndex index;

  ///index = pvTableView->selectionModel()->currentIndex();
  if(row >= 0){
    index = model()->index(row, pvDefaultColumnToSelect);
  }
  pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::ClearAndSelect);
}

void mdtSqlTableWidget::onDataEditionBegins()
{
  pvDelegateIsEditingData = true;
  emit dataEdited();
}

void mdtSqlTableWidget::onDataChanged(const QModelIndex &, const QModelIndex &)
{
  pvDelegateIsEditingData = false;
}

void mdtSqlTableWidget::onCurrentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
  if(!childWidgetsAreInVisaluzingState()){
    ///pvTableView->setCurrentIndex(previous);
    return;
  }
  if(current.isValid()){
    emit currentRowChanged(current.row());
  }else{
    emit currentRowChanged(-1);
  }
}

void mdtSqlTableWidget::onTableViewKnownKeyPressed(int key)
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  QModelIndex index;

  switch(key){
    case Qt::Key_Enter:
    case Qt::Key_Return:
      /*
       * We want to save data if we are in Editing or EditingNewRow state.
       * But, we must be shure that no editor is open in delegate to prevent data loss.
       * (If we call submit()/submitAll() on model, it will recieve invalid data
       *  for index that has a opened editor, and silently revert these data)
       */
      // Save data if we are in Editing state
      if((currentState() == Editing) || (currentState() == EditingNewRow)){
        if(!pvDelegateIsEditingData){
          submit();
        }
      }
      break;
    case Qt::Key_Down:
      // If we are at last row, we insert one - Only in visualizing state
      index = pvTableView->selectionModel()->currentIndex();
      if((index.row() == (model()->rowCount() - 1)) && (currentState() == Visualizing) && (pvTableView->editTriggers() != QAbstractItemView::NoEditTriggers)){
        // Insert new row and select it
        insert();
      }
      // Select new current index
      index = model()->index(index.row()+1, index.column());
      if(index.isValid()){
        switch(pvTableView->selectionBehavior()){
          case QAbstractItemView::SelectItems:
            pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear | QItemSelectionModel::Select);
            break;
          case QAbstractItemView::SelectRows:
            pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear | QItemSelectionModel::Select | QItemSelectionModel::Rows);
            break;
          case QAbstractItemView::SelectColumns:
            pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear | QItemSelectionModel::Select | QItemSelectionModel::Columns);
            break;
        }
      }
  }
}

void mdtSqlTableWidget::onModelSelected()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QModelIndex index;

  if(model() == 0){
    return;
  }
  if(model()->rowCount() > 0){
    index = model()->index(0, pvDefaultColumnToSelect);
    pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
    qDebug() << "mdtSqlTableWidget::onModelSelected() , selected index: " << index;
    pvTableView->resizeRowsToContents();
  }else{
    onCurrentRowChanged(index, index);
  }
}

void mdtSqlTableWidget::doSetModel(QSqlTableModel *model)
{
  Q_ASSERT(model != 0);

  model->setEditStrategy(QSqlTableModel::OnManualSubmit);
  pvTableView->setModel(model);
  connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onDataChanged(const QModelIndex&, const QModelIndex&)));
  // A selection model is created after setModel() by view itself
  Q_ASSERT(pvTableView->selectionModel() != 0);
  connect(pvTableView->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(onCurrentRowChanged(const QModelIndex&, const QModelIndex&)));
  pvDelegateIsEditingData = false;
}

bool mdtSqlTableWidget::doSubmit()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  if(!model()->submitAll()){
    displayDatabaseError(model()->lastError());
    return false;
  }

  return true;
}

bool mdtSqlTableWidget::doRevert()
{
  Q_ASSERT(model() != 0);

  model()->revertAll();

  return true;
}

bool mdtSqlTableWidget::doInsert()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  int row;
  QModelIndex index;

  // Insert new row at end
  row = model()->rowCount();
  model()->insertRow(row);
  index = model()->index(row, pvDefaultColumnToSelect);
  pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::SelectCurrent);
  qDebug() << "doInsert, selected index: " << index;

  return true;
}

bool mdtSqlTableWidget::doSubmitNewRow()
{
  return doSubmit();
}

bool mdtSqlTableWidget::doRevertNewRow()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  int row;

  // We simply remove the last row
  row = model()->rowCount()-1;
  if(row >= 0){
    if(!pvTableView->model()->removeRow(row)){
      displayDatabaseError(model()->lastError());
      return false;
    }
  }

  return true;
}

bool mdtSqlTableWidget::doRemove()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);
  Q_ASSERT(model() != 0);

  int i;
  int ret;
  int row;
  QList<int> rows;
  QMessageBox msgBox;
  QModelIndexList indexes = pvTableView->selectionModel()->selectedIndexes();

  // If nothing was selected, we do nothing
  if(indexes.size() < 1){
    return true;
  }
  // We ask confirmation to the user
  msgBox.setText(tr("You are about to delete selected rows in table ") + userFriendlyTableName());
  msgBox.setInformativeText(tr("Do you want to continue ?"));
  msgBox.setIcon(QMessageBox::Warning);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
  msgBox.setDefaultButton(QMessageBox::No);
  ret = msgBox.exec();
  if(ret != QMessageBox::Yes){
    return true;
  }
  // Build rows list
  for(i = 0; i < indexes.size(); ++i){
    row = indexes.at(i).row();
    if(!rows.contains(row)){
      rows.append(row);
    }
  }
  // Delete selected rows
  for(i = 0; i < rows.size(); ++i){
    if(!model()->removeRow(rows.at(i))){
      displayDatabaseError(model()->lastError());
      model()->revertAll();
      return false;
    }
  }
  if(!model()->submitAll()){
    model()->revert();
    displayDatabaseError(model()->lastError());
    return false;
  }

  return true;
}

void mdtSqlTableWidget::toFirst()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QModelIndex index;

  if(pvTableView->model() == 0){
    return;
  }
  index = pvTableView->model()->index(0, pvDefaultColumnToSelect);
  pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
}

void mdtSqlTableWidget::toLast()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QModelIndex index;
  int row;

  if(pvTableView->model() == 0){
    return;
  }
  index = pvTableView->selectionModel()->currentIndex();
  row = pvTableView->model()->rowCount()-1;
  if(index.row() < row){
    index = pvTableView->model()->index(row, pvDefaultColumnToSelect);
    pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
  }
}

void mdtSqlTableWidget::toPrevious()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QModelIndex index;
  int row;

  if(pvTableView->model() == 0){
    return;
  }
  // Get current index, check that we are not at the begining and move if Ok
  index = pvTableView->selectionModel()->currentIndex();
  if(index.row() > 0){
    row = index.row()-1;
    index = pvTableView->model()->index(row, pvDefaultColumnToSelect);
    pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
  }
}

void mdtSqlTableWidget::toNext()
{
  Q_ASSERT(pvTableView->selectionModel() != 0);

  QModelIndex index;
  int row;

  if(pvTableView->model() == 0){
    return;
  }
  // Get current index, check that we are not at the end and move if Ok
  index = pvTableView->selectionModel()->currentIndex();
  if(index.row() < (pvTableView->model()->rowCount())-1){
    row = index.row()+1;
    index = pvTableView->model()->index(row, pvDefaultColumnToSelect);
    pvTableView->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Clear);
  }
}

int mdtSqlTableWidget::firstVisibleColumnIndex()
{
  Q_ASSERT(model() != 0);

  int col;

  for(col = 0; col < model()->columnCount(); ++col){
    if(!pvTableView->isColumnHidden(col)){
      qDebug() << "firstVisibleColumnIndex(): " << col;
      return col;
    }
  }

  return -1;
}
