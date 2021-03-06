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
#include "mdtItemsSelectorDialog.h"
#include <QStringListModel>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QModelIndex>
#include <algorithm>
#include <vector>

//#include <QDebug>

mdtItemsSelectorDialog::mdtItemsSelectorDialog(QWidget *parent)
 : QDialog(parent)
{
  setupUi(this);
  pvSortOptionEnabled = false;
  // Setup items views
  pvAvailableItemsModel = new QStringListModel(this);
  lwAvailableItems->setSelectionMode(QAbstractItemView::MultiSelection);
  lwAvailableItems->setEditTriggers(QAbstractItemView::NoEditTriggers);
  lwAvailableItems->setModel(pvAvailableItemsModel);
  pvSelectedItemsModel = new QStringListModel(this);
  lwSelectedItems->setSelectionMode(QAbstractItemView::MultiSelection);
  lwSelectedItems->setEditTriggers(QAbstractItemView::NoEditTriggers);
  lwSelectedItems->setModel(pvSelectedItemsModel);
  // Setup sort combobox
  cbSortOrder->addItem(tr("Ascending"), Qt::AscendingOrder);
  cbSortOrder->addItem(tr("Descending"), Qt::DescendingOrder);

  connect(pbLeft, SIGNAL(clicked()), this, SLOT(transferSelectedItemsToSelectedItems()));
  connect(pbRight, SIGNAL(clicked()), this, SLOT(transferSelectedItemsToAvailableItems()));
  connect(pbSelectAllAvailableItems, SIGNAL(clicked()), this, SLOT(selectAllAvailableItems()));
  connect(pbUnselectAllAvailableItems, SIGNAL(clicked()), this, SLOT(unselectAllAvailableItems()));
  connect(pbSelectAllSelectedItems, SIGNAL(clicked()), this, SLOT(selectAllSelectedItems()));
  connect(pbUnselectAllSelectedItems, SIGNAL(clicked()), this, SLOT(unselectAllSelectedItems()));
  connect(pbUp, SIGNAL(clicked()), this, SLOT(moveSelectedItemsInSelectedItemsListUp()));
  connect(pbTop, SIGNAL(clicked()), this, SLOT(moveSelectedItemsInSelectedItemsListOnTop()));
  connect(pbDown, SIGNAL(clicked()), this, SLOT(moveSelectedItemsInSelectedItemsListDown()));
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);
}

mdtItemsSelectorDialog::~mdtItemsSelectorDialog()
{
}

void mdtItemsSelectorDialog::clear()
{
  pvAvailableItems.clear();
  pvSelectedItems.clear();
  updateAvailableItems(-1);
  updateSelectedItems(-1);
}

void mdtItemsSelectorDialog::setSortOptionEnabled(bool enabled)
{
  pvSortOptionEnabled = enabled;
  if(enabled){
    lbSortOrder->show();
    cbSortOrder->show();
    connect(lwSelectedItems->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),\
            this, SLOT(updateCbSortOrderBySelectedItem(const QModelIndex&, const QModelIndex&)));
    connect(cbSortOrder, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSortOrderOfCurrentItemInSelectedItemsList(int)));
  }else{
    lbSortOrder->hide();
    cbSortOrder->hide();
    disconnect(lwSelectedItems->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),\
            this, SLOT(updateCbSortOrderBySelectedItem(const QModelIndex&, const QModelIndex&)));
    disconnect(cbSortOrder, SIGNAL(currentIndexChanged(int)), this, SLOT(updateSortOrderOfCurrentItemInSelectedItemsList(int)));
  }
}

void mdtItemsSelectorDialog::setAvailableItemsLabelText(const QString &text)
{
  lbAvailableItems->setText(text);
}

void mdtItemsSelectorDialog::setAvailableItems(const QList<mdtItemsSelectorDialogItem> & items)
{
  pvAvailableItems = items;
  updateAvailableItems(-1);
}

void mdtItemsSelectorDialog::setSelectedItemsLabelText(const QString &text)
{
  lbSelectedItems->setText(text);
}

void mdtItemsSelectorDialog::setSelectedItems(const QList<mdtItemsSelectorDialogItem> & items)
{
  pvSelectedItems = items;
  updateSelectedItems(-1);
}

const QList<mdtItemsSelectorDialogItem> & mdtItemsSelectorDialog::selectedItems() const
{
  return pvSelectedItems;
}

QStringList mdtItemsSelectorDialog::selectedItemsTexts() const
{
  QStringList lst;
  int i;

  for(i = 0; i < pvSelectedItems.size(); ++i){
    lst.append(pvSelectedItems.at(i).text());
  }

  return lst;
}

QStringList mdtItemsSelectorDialog::selectedItemsNames() const
{
  QStringList lst;
  int i;

  for(i = 0; i < pvSelectedItems.size(); ++i){
    lst.append(pvSelectedItems.at(i).name());
  }

  return lst;
}

const QList<mdtItemsSelectorDialogItem> & mdtItemsSelectorDialog::notSelectedItems() const
{
  return pvAvailableItems;
}

void mdtItemsSelectorDialog::transferSelectedItemsToSelectedItems()
{
  Q_ASSERT(lwAvailableItems->selectionModel() != 0);

  QModelIndexList indexes = lwAvailableItems->selectionModel()->selectedIndexes();
  QModelIndex index;
  unsigned int ui;
  std::vector<int> rowList;
  mdtItemsSelectorDialogItem item;

  // Build list of rows to remove from available items
  foreach(index, indexes){
    rowList.push_back(index.row());
  }
  // Transfer items
  for(ui = 0; ui < rowList.size(); ++ui){
    Q_ASSERT(rowList[ui] < pvAvailableItems.size());
    item = pvAvailableItems.at(rowList[ui]);
    pvSelectedItems.append(item);
  }
  // Remove transfered items
  std::sort(rowList.rbegin(), rowList.rend());
  for(ui = 0; ui < rowList.size(); ++ui){
    Q_ASSERT(rowList[ui] < pvAvailableItems.size());
    pvAvailableItems.removeAt(rowList[ui]);
  }
  // Update models
  updateAvailableItems(-1);
  updateSelectedItems(-1);
}

void mdtItemsSelectorDialog::transferSelectedItemsToAvailableItems()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndexList indexes = lwSelectedItems->selectionModel()->selectedIndexes();
  QModelIndex index;
  unsigned int ui;
  std::vector<int> rowList;
  mdtItemsSelectorDialogItem item;

  // Build list of rows to remove from selected items
  foreach(index, indexes){
    rowList.push_back(index.row());
  }
  // Transfer items
  for(ui = 0; ui < rowList.size(); ++ui){
    Q_ASSERT(rowList[ui] < pvSelectedItems.size());
    item = pvSelectedItems.at(rowList[ui]);
    pvAvailableItems.append(item);
  }
  // Remove transfered items
  std::sort(rowList.rbegin(), rowList.rend());
  for(ui = 0; ui < rowList.size(); ++ui){
    Q_ASSERT(rowList[ui] < pvSelectedItems.size());
    pvSelectedItems.removeAt(rowList[ui]);
  }
  // Update models
  updateAvailableItems(-1);
  updateSelectedItems(-1);
}

void mdtItemsSelectorDialog::selectAllAvailableItems()
{
  Q_ASSERT(lwAvailableItems->selectionModel() != 0);

  QModelIndex topIndex = pvAvailableItemsModel->index(0, 0);
  QModelIndex bottomIndex = pvAvailableItemsModel->index(pvAvailableItemsModel->rowCount()-1, 0);
  QItemSelection selection(topIndex, bottomIndex);

  lwAvailableItems->selectionModel()->select(selection, QItemSelectionModel::Select);
}

void mdtItemsSelectorDialog::unselectAllAvailableItems()
{
  Q_ASSERT(lwAvailableItems->selectionModel() != 0);

  QModelIndex topIndex = pvAvailableItemsModel->index(0, 0);
  QModelIndex bottomIndex = pvAvailableItemsModel->index(pvAvailableItemsModel->rowCount()-1, 0);
  QItemSelection selection(topIndex, bottomIndex);

  lwAvailableItems->selectionModel()->select(selection, QItemSelectionModel::Deselect);
}

void mdtItemsSelectorDialog::selectAllSelectedItems()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndex topIndex = pvSelectedItemsModel->index(0, 0);
  QModelIndex bottomIndex = pvSelectedItemsModel->index(pvSelectedItemsModel->rowCount()-1, 0);
  QItemSelection selection(topIndex, bottomIndex);

  lwSelectedItems->selectionModel()->select(selection, QItemSelectionModel::Select);
}

void mdtItemsSelectorDialog::unselectAllSelectedItems()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndex topIndex = pvSelectedItemsModel->index(0, 0);
  QModelIndex bottomIndex = pvSelectedItemsModel->index(pvSelectedItemsModel->rowCount()-1, 0);
  QItemSelection selection(topIndex, bottomIndex);

  lwSelectedItems->selectionModel()->select(selection, QItemSelectionModel::Deselect);
}

void mdtItemsSelectorDialog::moveSelectedItemsInSelectedItemsListUp()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndex index = lwSelectedItems->selectionModel()->currentIndex();
  int row;

  if(!index.isValid()){
    return;
  }
  row = index.row();
  moveSelectedItem(row, row - 1, true);
}

void mdtItemsSelectorDialog::moveSelectedItemsInSelectedItemsListOnTop()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndex index;
  int row;


  index = lwSelectedItems->selectionModel()->currentIndex();
  if(!index.isValid()){
    return;
  }
  row = index.row();
  moveSelectedItem(row, 0, true);
}

void mdtItemsSelectorDialog::moveSelectedItemsInSelectedItemsListDown()
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndex index = lwSelectedItems->selectionModel()->currentIndex();
  int row;

  if(!index.isValid()){
    return;
  }
  row = index.row();
  moveSelectedItem(row, row + 1, true);
}

void mdtItemsSelectorDialog::updateCbSortOrderBySelectedItem(const QModelIndex &current, const QModelIndex &previous)
{
  int row;

  if(!current.isValid()){
    cbSortOrder->setEnabled(false);
    return;
  }
  row = current.row();
  if((row >= 0)&&(row < pvSelectedItems.size())){
    if(pvSelectedItems.at(row).sortOrder() == Qt::AscendingOrder){
      cbSortOrder->setCurrentIndex(0);
    }else{
      cbSortOrder->setCurrentIndex(1);
    }
  }
}

void mdtItemsSelectorDialog::updateSortOrderOfCurrentItemInSelectedItemsList(int cbSortOrderIndex)
{
  Q_ASSERT(lwSelectedItems->selectionModel() != 0);

  QModelIndex index = lwSelectedItems->selectionModel()->currentIndex();
  int row;

  if(!index.isValid()){
    return;
  }
  row = index.row();
  Q_ASSERT(row >= 0);
  Q_ASSERT(row < pvSelectedItems.size());
  switch(cbSortOrderIndex){
    case 0:
      pvSelectedItems[row].setSortOrder(Qt::AscendingOrder);
      break;
    case 1:
      pvSelectedItems[row].setSortOrder(Qt::DescendingOrder);
      break;
  }
  updateSelectedItems(row);
}

void mdtItemsSelectorDialog::moveSelectedItem(int itemRow, int newRow, bool selectNewRow)
{
  mdtItemsSelectorDialogItem item;

  // Check indexes
  if((itemRow < 0)||(itemRow >= pvSelectedItems.size())){
    return;
  }
  if((newRow < 0)||(newRow >= pvSelectedItems.size())){
    return;
  }
  // Move item
  pvSelectedItems.move(itemRow, newRow);
  // Update model
  if(selectNewRow){
    updateSelectedItems(newRow);
  }else{
    updateSelectedItems(-1);
  }
}


void mdtItemsSelectorDialog::updateAvailableItems(int currentRow)
{
  QStringList itemsTexts;
  QModelIndex index;
  int i;

  qSort(pvAvailableItems.begin(), pvAvailableItems.end());
  for(i = 0; i < pvAvailableItems.size(); ++i){
    itemsTexts.append(pvAvailableItems.at(i).text());
  }
  pvAvailableItemsModel->setStringList(itemsTexts);
  // Update selection
  if(lwAvailableItems->selectionModel() != 0){
    unselectAllAvailableItems();
    index = pvAvailableItemsModel->index(currentRow, 0);
    lwAvailableItems->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
  }
}

void mdtItemsSelectorDialog::updateSelectedItems(int currentRow)
{
  QStringList itemsTexts;
  QModelIndex index;
  int i;

  for(i = 0; i < pvSelectedItems.size(); ++i){
    itemsTexts.append(pvSelectedItems.at(i).text(pvSortOptionEnabled));
  }
  pvSelectedItemsModel->setStringList(itemsTexts);
  // Update selection
  if(lwSelectedItems->selectionModel() != 0){
    unselectAllSelectedItems();
    index = pvSelectedItemsModel->index(currentRow, 0);
    lwSelectedItems->selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
  }
}
