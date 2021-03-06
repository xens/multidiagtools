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
#ifndef MDT_ITEMS_SELECTOR_DIALOG_H
#define MDT_ITEMS_SELECTOR_DIALOG_H

#include "ui_mdtItemsSelectorDialog.h"
#include "mdtItemsSelectorDialogItem.h"
#include <QDialog>
#include <QWidget>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>
#include <QPair>

class QStringListModel;

/*! \brief Provide the user a dialog in witch he can choose items
 */
class mdtItemsSelectorDialog : public QDialog, Ui::mdtItemsSelectorDialog
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtItemsSelectorDialog(QWidget *parent = 0);

  /*! \brief Destructor
   */
  ~mdtItemsSelectorDialog();

  /*! \brief Clear dialog
   */
  void clear();

  /*! \brief Enable/diseable sort option
   */
  void setSortOptionEnabled(bool enabled);

  /*! \brief Set the Available items label text
   */
  void setAvailableItemsLabelText(const QString &text);

  /*! \brief Set the Available items
   */
  void setAvailableItems(const QList<mdtItemsSelectorDialogItem> & items);

  /*! \brief Set the selected items label text
   */
  void setSelectedItemsLabelText(const QString &text);

  /*! \brief Set the Available items
   */
  void setSelectedItems(const QList<mdtItemsSelectorDialogItem> & items);

  /*! \brief Get list of selected items
   */
  const QList<mdtItemsSelectorDialogItem> & selectedItems() const;

  /*! \brief Get list of selected items texts
   */
  QStringList selectedItemsTexts() const;

  /*! \brief Get list of selected items names
   */
  QStringList selectedItemsNames() const;

  /*! \brief Get list of not selected items
   */
  const QList<mdtItemsSelectorDialogItem> & notSelectedItems() const;

 private slots:

  /*! \brief Transfer selected items from available to selected
   */
  void transferSelectedItemsToSelectedItems();

  /*! \brief Transfer selected items from selected to available
   */
  void transferSelectedItemsToAvailableItems();

  /*! \brief Select all items in available list
   */
  void selectAllAvailableItems();

  /*! \brief Unselect all items in available list
   */
  void unselectAllAvailableItems();

  /*! \brief Select all items in selected list
   */
  void selectAllSelectedItems();

  /*! \brief Unselect all items in selected list
   */
  void unselectAllSelectedItems();

  /*! \brief Move selected items in selected list up
   */
  void moveSelectedItemsInSelectedItemsListUp();

  /*! \brief Move selected items in selected list on top
   */
  void moveSelectedItemsInSelectedItemsListOnTop();

  /*! \brief Move selected items in selected list down
   */
  void moveSelectedItemsInSelectedItemsListDown();

  /*! \brief Update cbSortOrder (enable/diseable and sort order regarding in selected list)
   */
  void updateCbSortOrderBySelectedItem(const QModelIndex &current, const QModelIndex &previous);

  /*! \brief Update sort order (ascending/desending) for current selected item
   */
  void updateSortOrderOfCurrentItemInSelectedItemsList(int cbSortOrderIndex);

 private:

  /*! \brief Move a item from selected items list
   *
   * Will move item at row itemRow to newRow.
   *  If itemRow or newRow is out of range,
   *  this method does simply nothing.
   */
  void moveSelectedItem(int itemRow, int newRow, bool selectNewRow);

  /*! \brief Update available items model and view
   */
  void updateAvailableItems(int currentRow);

  /*! \brief Update selected items model and view
   */
  void updateSelectedItems(int currentRow);

  Q_DISABLE_COPY(mdtItemsSelectorDialog);

  QStringListModel *pvAvailableItemsModel;
  QStringListModel *pvSelectedItemsModel;
  QList<mdtItemsSelectorDialogItem> pvAvailableItems;
  QList<mdtItemsSelectorDialogItem> pvSelectedItems;
  bool pvSortOptionEnabled;
};

#endif  // #ifndef MDT_ITEMS_SELECTOR_DIALOG_H
