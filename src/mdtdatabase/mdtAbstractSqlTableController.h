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
#ifndef MDT_ABSTRACT_SQL_TABLE_CONTROLLER_H
#define MDT_ABSTRACT_SQL_TABLE_CONTROLLER_H

#include "mdtError.h"
#include "mdtStateMachine.h"
#include "mdtUiMessageHandler.h"
#include "mdtSqlDataValidator.h"
#include "mdtSqlRelationInfo.h"
#include "mdtSortFilterProxyModel.h"
#include "mdtFormatProxyModelSettings.h"
#include "mdtSqlRecord.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QList>
#include <memory>
#include <vector>

class mdtState;
class mdtAbstractSqlTableController;
class mdtSqlRelation;
class mdtFormatProxyModel;
class mdtSortFilterProxyModel;
class QSqlField;

/*! \internal Container for internal use
 */
struct mdtAbstractSqlTableControllerContainer
{
  std::shared_ptr<mdtAbstractSqlTableController> controller;
  std::shared_ptr<mdtSqlRelation> relation;
  mdtSqlRelationInfo::relationType_t relationType;
};

/*! \brief Base class for SQL table controllers
 *
 * In MVC pattern, this class is the controller part for database GUI.
 *  \todo Documnent correctly
 *
 * When creating a GUI for database table edition,
 *  this class can be used as controller between your GUI and QSqlTableModel.
 *  To keep coherence of common operation, such as insert, remove, submit and remove,
 *  some signals are available:
 *   - insertEnabledStateChanged()
 *   - removeEnabledStateChanged()
 *   - submitEnabledStateChanged()
 *   - removeEnabledStateChanged()
 *
 * This abstract class can not be used directly, but must be subclassed.
 *  Use existing one, or create your own if needed.
 *
 * Typicall usage:
 *  - Make needed signal/slot connections with GUI actions
 *  - Set table with setTableName()
 *  - If needed, add child controllers with addChildController()
 *  - Start inetrnal state machine with start()
 *  - Load data with select()
 */
class mdtAbstractSqlTableController : public QObject
{
  Q_OBJECT

 public:

  /*! \brief State of edit/view widget
   *
   * This state is set by internal state machine.
   */
  enum State_t {
                Stopped = 0,      /*!< Stopped state (internall state machine not running, see start() ) */
                Selecting ,       /*!< Selecting state */
                Visualizing ,     /*!< Visualizing state */
                Reverting,        /*!< Reverting state */
                Editing,          /*!< Editing state */
                Submitting,       /*!< Submitting state */
                Inserting,        /*!< Inserting state */
                EditingNewRow,    /*!< Editing a new row */
                RevertingNewRow,  /*!< RevertingNewRow state */
                SubmittingNewRow, /*!< SubmittingNewRow state */
                Removing          /*!< Removing state */
               };

  /*! \brief Constructor
   */
  mdtAbstractSqlTableController(QObject * parent = 0);

  /*! \brief Destructor
   */
  virtual ~mdtAbstractSqlTableController();

  /*! \brief Set message handler
   *
   * Will also set this message handler to each contained data validator (see addDataValidator() ).
   */
  void setMessageHandler(std::shared_ptr<mdtUiMessageHandler> handler);

  /*! \brief Check if a message handler was allready set
   */
  bool hasMessageHandler() const;

  /*! \brief Set table name
   *
   * Will replace internal model if allready set.
   */
  void setTableName(const QString & tableName, QSqlDatabase db, const QString & userFriendlyTableName = QString());

  /*! \brief Set table model
   *
   * Will replace internal model if allready set.
   *
   * Note that one of the goal of a SQL table controller is to prevent from silent update of database
   *  by making checks before submitting data and warn the user if something is wrong.
   *  Keeping GUI coherent with current state is also a goal.
   *  For this reasons, the model's edit strategy will be set to QSqlTableModel::OnManualSubmit.
   */
  void setModel(std::shared_ptr<QSqlTableModel> m, const QString & userFriendlyTableName = QString());

  /*! \brief Set the canWriteToDatabase flag
   *
   * If this flag is true (the default),
   *  calling submit will send data to database
   *  (see QSqlTableModel::submitAll() for details).
   *  For some cases, typically when working on a view,
   *  it's not possible to store data to DB by a default method.
   *  A way to deal with this is to only cache data to model,
   *  and save data to each corresponding tables in a manual way.
   *  For such usage, set this flag false.
   */
  void setCanWriteToDatabase(bool canWrite);

  /*! \brief Set the canWriteToDatabase flag
   *
   * \sa setCanWriteToDatabase().
   */
  inline bool canWriteToDatabase() const
  {
    return pvCanWriteToDatabase;
  }

  /*! \brief Get index of field with fieldName
   *
   * Returns a value < 0 if fieldName was not found.
   */
  int fieldIndex(const QString & fieldName) const;

  /*! \brief Set a user friendly name for a column
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  void setHeaderData(const QString &fieldName, const QString &data);

  /*! \brief Get header data of given section
   */
  inline QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
  {
    return pvProxyModel->headerData(section, orientation, role);
  }

  /*! \brief Get header data of given column
   */
  inline QVariant headerData(int column, int role = Qt::DisplayRole) const
  {
    return pvProxyModel->headerData(column, Qt::Horizontal, role);
  }

  /*! \brief Get header data of given field name
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QVariant headerData(const QString & fieldName, int role = Qt::DisplayRole) const;

  /*! \brief Get header row data of given columns
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QList<QVariant> headerRowData(const std::vector<int> & columns, int role = Qt::DisplayRole) const;

  /*! \brief Get header row data of given field names
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QList<QVariant> headerRowData(const QStringList & fieldNames, int role = Qt::DisplayRole) const
  {
    return headerRowData(columnList(fieldNames), role);
  }

  /*! \brief Get header row data of given columns
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QStringList headerRowDataStr(const std::vector<int> & columns, int role = Qt::DisplayRole) const;

  /*! \brief Get header row data of given field names
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QStringList headerRowDataStr(const QStringList & fieldNames, int role = Qt::DisplayRole) const
  {
    return headerRowDataStr(columnList(fieldNames), role);
  }

  /*! \brief Add a child controller
   *
   * Will create a controller of type T, needed relations (mdtSqlRelation objects)
   *  and add controller to childs.
   *
   * \param relationInfo Informations of relation between current controller's table (parent) and child controller's table (child)
   *               Note: parent table name is ignored.
   * \param db QSqlDatabase object to use
   * \param userFriendlyChildTableName User friendly table name for child controller.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  template<typename T>
  bool addChildController(const mdtSqlRelationInfo & relationInfo, QSqlDatabase db, const QString & userFriendlyChildTableName = QString())
  {
    Q_ASSERT(pvModel);
    std::shared_ptr<T> controller(new T);
    return setupAndAddChildController(controller, relationInfo, db, userFriendlyChildTableName);
  }

  /*! \brief Add a child controller
   *
   * Will create a controller of type T, needed relations (mdtSqlRelation objects)
   *  and add controller to childs.
   *
   * \param relationInfo Informations of relation between current controller's table (parent) and child controller's table (child)
   *               Note: parent table name is ignored.
   * \param userFriendlyChildTableName User friendly table name for child controller.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  template<typename T>
  bool addChildController(const mdtSqlRelationInfo & relationInfo, const QString & userFriendlyChildTableName = QString())
  {
    Q_ASSERT(pvModel);
    return addChildController<T>(relationInfo, pvModel->database(), userFriendlyChildTableName);
  }

  /*! \brief Get a child controller
   *
   *  Can return a Null pointer if no controller
   *   is assigned to requested table, or it exists,
   *   but is not requested type T.
   */
  template<typename T>
  std::shared_ptr<T> childController(const QString & tableName)
  {
    int i;
    std::shared_ptr<mdtAbstractSqlTableController> c;
    for(i = 0; i < pvChildControllerContainers.size(); ++i){
      c = pvChildControllerContainers.at(i).controller;
      Q_ASSERT(c);
      if(c->tableName() == tableName){
        return std::dynamic_pointer_cast<T>(c);
      }
      // Try in child controller's childs
      auto tc = c->childController<T>(tableName);
      if(tc){
        return tc;
      }
    }
    return std::shared_ptr<T>();
  }

  /*! \brief Select data in main table
   *
   * \pre Table model must be set with setModel() or setTableName() before calling this method.
   * \pre Internal state machine must run (see start() ).
   */
  bool select();

  /*! \brief Refresh data of current row
   *
   * This is the same as calling setCurrentRow(currentRow()).
   */
  bool refresh()
  {
    return setCurrentRow(currentRow());
  }

  /*! \brief Set a filter on main table
   *
   * Set filter based on SQL query WHERE part, without the WHERE keyword (f.ex. Id_PK = 15)
   *
   *  Note: if internal controller's model is allready populated with data,
   *  i.e. select() was called before, filter is applied directly, else,
   *  select must be called explicitly after setMainTableFilter()
   *  (this is the same behaviour as QSqlTableModel).
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Internal state machine must run (see start() ).
   */
  bool setFilter(const QString & filter);

  /*! \brief Set a filter on main table
   *
   * \param fieldName Field that must match data
   * \param matchData Match data
   *
   * Internally, a SQL statement is generated linke: fieldName = matchData
   *
   *  Note: if internal controller's model is allready populated with data,
   *  i.e. select() was called before, filter is applied directly, else,
   *  select must be called explicitly after setMainTableFilter()
   *  (this is the same behaviour as QSqlTableModel).
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Internal state machine must run (see start() ).
   */
  bool setFilter(const QString & fieldName, const QVariant & matchData);

  /*! \brief Set a filter on main table
   *
   * \param fieldName Field that must match data
   * \param matchDataList A list of match data
   *
   * Internally, a SQL statement is generated linke: fieldName IN (matchData[0], matchData[1], ...)
   *
   *  Note: if internal controller's model is allready populated with data,
   *  i.e. select() was called before, filter is applied directly, else,
   *  select must be called explicitly after setMainTableFilter()
   *  (this is the same behaviour as QSqlTableModel).
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Internal state machine must run (see start() ).
   */
  bool setFilter(const QString & fieldName, const QList<QVariant> & matchDataList);

  /*! \brief Clear filter
   */
  void clearFilter();

  /*! \brief Get model (read only access)
   */
  inline const std::shared_ptr<QSqlTableModel> model() const { return pvModel; }

  /*! \brief Get model (write access)
   *
   * \todo If not needed, make it protected and adapt tests
   */
  inline std::shared_ptr<QSqlTableModel> model() { return pvModel; }

  /*! \brief Get (technical) table name
   */
  QString tableName() const;

  /*! \brief Set user friendly table name
   */
  void setUserFriendlyTableName(const QString & name) { pvUserFriendlyTableName = name; }

  /*! \brief Get user friendly table name
   */
  QString userFriendlyTableName() const { return pvUserFriendlyTableName; }

  /*! \brief Start internal state machine
   *
   * \pre Table model must be set with setModel() or setTableName() before calling this method.
   */
  void start();

  /*! \brief Stop internal state machine
   *
   * \pre Table model must be set with setModel() or setTableName() before calling this method.
   */
  void stop();

  /*! \brief Get current state
   */
  inline State_t currentState() const {
    if(!pvStateMachine.isRunning()){
      return Stopped;
    }
    return static_cast<State_t>(pvStateMachine.currentState());
  }

  /*! \brief Add a data validator
   *
   * \param validator A instance of mdtSqlDataValidator or subclass.
   * \param putAtTopPriority If true, validator will be called before allready stored ones.
   *                          (validators are put into a queue).
   * \param setInternalMessageHandler If true, message handler set with setMessageHandler() will be set to validator.
   *
   * \pre validator must be a valid pointer.
   */
  void addDataValidator(std::shared_ptr<mdtSqlDataValidator> validator, bool putAtTopPriority = false, bool setInternalMessageHandler = true);

  /*! \brief Set format settings
   */
  void setFormatSettings(const mdtFormatProxyModelSettings & settings);

  /*! \brief Add a text alignment fromat for given column
   *
   * Note: if given fieldName does not exist,
   *  this function silently adds nothing.
   */
  void addTextAlignmentFormat(const QString & fieldName, Qt::Alignment alignment);

  /*! \brief Set column that contains key data to determine backgroud fromat
   *
   * Note: if given fieldName does not exist,
   *  this function silently updates nothing.
   */
  void setBackgroundFromatKeyDataColumn(const QString & fieldName);

  /*! \brief Add a background format for given key data
   */
  void addBackgroundFromat(const QVariant & keyData, const QBrush & bg);

  /*! \brief Clear format settings
   */
  void clearFormatSettings();

  /*! \brief Add a column to columns sort order
   *
   * Columns sort order is similar meaning
   *  than SQL ORDER BY clause.
   *
   * Internally, mdtSortFilterProxyModel is used, witch provide a natural sort for strings.
   *
   * For example, to sort columns in order "Id_PK", "FirstName", "LastName", all ascending, call:
   *  - clearColumnsSortOrder();
   *  - addColumnToSortOrder("Id_PK", Qt::AscendingOrder);
   *  - addColumnToSortOrder("FirstName", Qt::AscendingOrder);
   *  - addColumnToSortOrder("LastName", Qt::AscendingOrder);
   *
   * Note: if given field not exists, it will simply be ignored.
   *
   * Note: to apply sorting, call sort()
   *
   * \pre Model must be set with setModel() before using this method.
   */
  void addColumnToSortOrder(const QString & fieldName, Qt::SortOrder order = Qt::AscendingOrder);

  /*! \brief Clear columns sort order
   *
   * \sa addColumnToSortOrder()
   */
  void clearColumnsSortOrder();

  /*! \brief Sort data
   *
   * \sa addColumnToSortOrder()
   */
  void sort();

  /*! \brief Set current data for given field name
   *
   * Will update current data for given fieldName in internal model.
   *  If given fieldName does not exists, lastError will contain a message on index error.
   *
   * If canWriteToDatabase is true, currentState will be updated, else not.
   *
   * Note: if submit is true, but canWriteToDatabase flag is false,
   *  submit flag is silently ignored.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool setCurrentData(const QString &fieldName, const QVariant &data, bool submit = true);

  /*! \brief Set data for given row and field name
   *
   * Will update data at given row and fieldName in internal model.
   *  Index checking is done internally, and lastError will contain a message on index error.
   *
   * If canWriteToDatabase is true, currentState will be updated, else not.
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * Note: if submit is true, but canWriteToDatabase flag is false,
   *  submit flag is silently ignored.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool setData(int row, const QString &fieldName, const QVariant &data, bool submit = true);

  /*! \brief Set data for given match and field name
   *
   * \param matchFieldName Field name in witch matchData must be searched
   * \param matchData Data that must match in matchFieldName
   * \param dataFieldName Field name in witch data must be set
   * \param data Data to set
   * \param submit If true, data will be submitted to database, else it will be cached only in model.
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \sa setData(int row, const QString&, const QVariant&, bool)
   */
  bool setData(const QString & matchFieldName, const QVariant & matchData, const QString & dataFieldName, const QVariant &data, bool submit = true);

  /*! \brief Set data for given match and field name
   *
   * \param matchFieldName1 Field name in witch key1 must be searched
   * \param key1 Data that must match in matchFieldName1
   * \param matchFieldName2 Field name in witch key2 must be searched
   * \param key2 Data that must match in matchFieldName2
   * \param dataFieldName Field name in witch data must be set
   * \param data Data to set
   * \param submit If true, data will be submitted to database, else it will be cached only in model.
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \sa setData(int row, const QString&, const QVariant&, bool)
   */
  bool setData(const QString & matchFieldName1, const QVariant & key1, const QString & matchFieldName2, const QVariant & key2,
               const QString & dataFieldName, const QVariant &data, bool submit = true);

  /*! \brief Get current data for given field name
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant currentData(const QString &fieldName)
  {
    Q_ASSERT(pvModel);
    if(currentRow() < 0){
      return QVariant();
    }
    return data(currentRow(), fieldName);
  }

  /*! \brief Get current data for given field name
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant currentData(const QString &fieldName, bool & ok)
  {
    Q_ASSERT(pvModel);
    if(currentRow() < 0){
      return QVariant();
    }
    return data(currentRow(), fieldName, ok);
  }

  /*! \brief Get formated current value for SQL query
   */
  inline QString currentFormatedValue(const QString & fieldName)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return currentFormatedValue(fieldName, ok);
  }

  /*! \brief Get formated current value for SQL query
   */
  inline QString currentFormatedValue(const QString & fieldName, bool & ok)
  {
    Q_ASSERT(pvModel);
    return formatedValue(currentRow(), fieldName, ok);
  }

  /*! \brief Get data for given index
   *
   * Note: index is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant data(const QModelIndex & index)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return data(index, ok);
  }

  /*! \brief Get data for given index
   *
   * Note: index is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QVariant data(const QModelIndex & index, bool & ok);

  /*! \brief Get data for given row and column
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant data(int row, int column)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return data(row, column, ok);
  }

  /*! \brief Get data for given row and column
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant data(int row, int column, bool & ok)
  {
    Q_ASSERT(pvModel);
    return data(pvProxyModel->index(row, column), ok);
  }

  /*! \brief Get data for given row and field name
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QVariant data(int row, const QString &fieldName)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return data(row, fieldName, ok);
  }

  /*! \brief Get data for given row and field name
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QVariant data(int row, const QString &fieldName, bool & ok);

  /*! \brief Get data for given match and field name
   *
   * \param matchFieldName Field name in witch matchData must be searched
   * \param matchData Data that must match in matchFieldName
   * \param dataFieldName Field name in witch data must be returned
   */
  QVariant data(const QString & matchFieldName, const QVariant & matchData, const QString & dataFieldName, bool & ok);

  /*! \brief Get data for given match and field name
   *
   * \param matchFieldName Field name in witch matchData must be searched
   * \param matchData Data that must match in matchFieldName
   * \param dataFieldName Field name in witch data must be returned
   */
  inline QVariant data(const QString & matchFieldName, const QVariant & matchData, const QString & dataFieldName)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return data(matchFieldName, matchData, dataFieldName, ok);
  }

  /*! \brief Get data for given match and field name
   *
   * \param matchFieldName1 Field name in witch key1 must be searched
   * \param key1 Data that must match in matchFieldName1
   * \param matchFieldName2 Field name in witch key2 must be searched
   * \param key2 Data that must match in matchFieldName2
   * \param dataFieldName Field name in witch data must be returned
   */
  QVariant data(const QString & matchFieldName1, const QVariant & key1, const QString & matchFieldName2, const QVariant & key2, const QString & dataFieldName, bool & ok);

  /*! \brief Get data for given match and field name
   *
   * \param matchFieldName1 Field name in witch key1 must be searched
   * \param key1 Data that must match in matchFieldName1
   * \param matchFieldName2 Field name in witch key2 must be searched
   * \param key2 Data that must match in matchFieldName2
   * \param dataFieldName Field name in witch data must be returned
   */
  inline QVariant data(const QString & matchFieldName1, const QVariant & key1, const QString & matchFieldName2, const QVariant & key2, const QString & dataFieldName)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return data(matchFieldName1, key1, matchFieldName2, key2, dataFieldName, ok);
  }

  /*! \brief Get row data of given row and columns list
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Given row and columns must exist in table.
   */
  QList<QVariant> rowData(int row, const std::vector<int> & columns, int role = Qt::DisplayRole) const;

  /*! \brief Get row data of given row and field names
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Given row and field names must exist in table.
   */
  QList<QVariant> rowData(int row, const QStringList & fieldNames, int role = Qt::DisplayRole) const
  {
    return rowData(row, columnList(fieldNames), role);
  }

  /*! \brief Get row data of given row and field names
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Given row and field names must exist in table.
   */
  QStringList rowDataStr(int row, const std::vector<int> & columns, int role = Qt::DisplayRole) const;

  /*! \brief Get row data of given row and field names
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Given row and field names must exist in table.
   */
  QStringList rowDataStr(int row, const QStringList & fieldNames, int role = Qt::DisplayRole) const
  {
    return rowDataStr(row, columnList(fieldNames), role);
  }

  /*! \brief Get a record that contains given field names for given row
   */
  inline mdtSqlRecord record(int row, const QStringList & fieldNames)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return record(row, fieldNames, ok);
  }

  /*! \brief Get a record that contains given field names for given row
   */
  mdtSqlRecord record(int row, const QStringList & fieldNames, bool & ok);

  /*! \brief Get a record for given match and that contains given field names
   *
   * \param matchFieldName Field name in witch matchData must be searched
   * \param matchData Data that must match in matchFieldName
   * \param fieldNames List of fields that must be contained in returned record
   */
  mdtSqlRecord record(const QString & matchFieldName, const QVariant & matchData, const QStringList & fieldNames, bool & ok);

  /*! \brief Get a record for given match and that contains given field names
   *
   * \param matchFieldName Field name in witch matchData must be searched
   * \param matchData Data that must match in matchFieldName
   * \param fieldNames List of fields that must be contained in returned record
   */
  inline mdtSqlRecord record(const QString & matchFieldName, const QVariant & matchData, const QStringList & fieldNames)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return record(matchFieldName, matchData, fieldNames, ok);
  }

  /*! \brief Get a record that contains given field names for current row
   */
  inline mdtSqlRecord currentRecord(const QStringList & fieldNames, bool & ok)
  {
    Q_ASSERT(pvModel);
    return record(currentRow(), fieldNames, ok);
  }

  /*! \brief Get a record that contains given field names for current row
   */
  inline mdtSqlRecord currentRecord(const QStringList & fieldNames)
  {
    Q_ASSERT(pvModel);
    return record(currentRow(), fieldNames);
  }

  /*! \brief Get a list of data for given field name
   *
   * If fetchAll is true, all available data will be fetched from database
   *  (regarding applied filter), else only cached data are returned
   *  (see QSqlQueryModel::fetchMore() for details).
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  inline QList<QVariant> dataList(const QString &fieldName, bool fetchAll = true)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return dataList(fieldName, ok, fetchAll);
  }

  /*! \brief Get a list of data for given field name
   *
   * If fetchAll is true, all available data will be fetched from database
   *  (regarding applied filter), else only cached data are returned
   *  (see QSqlQueryModel::fetchMore() for details).
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  QList<QVariant> dataList(const QString &fieldName, bool & ok, bool fetchAll = true);

  /*! \brief Get formated value for SQL query
   */
  inline QString formatedValue(int row, const QString & fieldName)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return formatedValue(row, fieldName, ok);
  }

  /*! \brief Get formated value for SQL query
   *
   * Will add delimiter to value,
   *  regarding field type.
   */
  QString formatedValue(int row, const QString & fieldName, bool & ok);

  /*! \brief Get formated value for SQL query
   *
   * Will add delimiter to value,
   *  regarding field type.
   */
  inline QString formatValue(const QString & fieldName, const QVariant & value)
  {
    Q_ASSERT(pvModel);
    bool ok;
    return formatValue(fieldName, value, ok);
  }

  /*! \brief Get formated value for SQL query
   *
   * Will add delimiter to value,
   *  regarding field type.
   */
  QString formatValue(const QString & fieldName, const QVariant & value, bool & ok);

  /*! \brief Get formated value for SQL query
   *
   * Will add delimiter to value,
   *  regarding field type.
   */
  QString formatValue(const QSqlField & field);

  /*! \brief Get column count
   */
  inline int columnCount() const { return pvProxyModel->columnCount(); }

  /*! \brief Get row count
   *
   * \param fetchAll If true, all rows will be fetched in database (see QSqlTableModel::fetchMore() )
   */
  int rowCount(bool fetchAll = true) const;

  /*! \brief Set current row
   *
   * If row is > model's cached rowCount-1, data will be fetched
   *  in database. If request row could not be found, false is returned.
   *
   * \param row Row to witch to go. Must be in range [-1;rowCount()-1]
   *            Note: row is relative to sorted data model (proxyModel),
   *             not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool setCurrentRow(int row);

  /*! \brief Set current row
   *
   * Will search the first record that contains
   *  match data for given field name.
   *
   * \sa setCurrentRow(int).
   */
  bool setCurrentRow(const QString & fieldName, const QVariant & matchData);

  /*! \brief Get current row
   */
  inline int currentRow() const { return pvCurrentRow; }

  /*! \brief Update controllers with data for current row
   *
   * \pre Table model must be set with setModel() or setTableName() before calling this method.
   */
  void update();

  /*! \brief Check if all data are saved
   *
   * This will check state of current table controller
   *  and state of all assigned child controllers.
   *  If one of them is not in Visualizing, Selecting or Stopped state,
   *  some data are not saved.
   *
   * If checkAboutDirtyIndex is true, current state of controllers are also checked,
   *  but in addition, each index for each cached row will be checked using QSqlTableModel::isDirty().
   *  This is slower, but works also after data was edited with setData().
   *
   * \return true if all data are saved, false if there are pending data.
   */
  bool allDataAreSaved(bool checkAboutDirtyIndex = false);

  /*! \brief Get last error
   */
  mdtError lastError() const { return pvLastError; }

  /*! \brief Submit and wait until success or failure
   *
   *  Note: will not freeze Qt's event loop.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool submitAndWait();

  /*! \brief Remove and wait until success or failure
   *
   *  Note: will not freeze Qt's event loop.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool removeAndWait();

  /*! \brief Insert a new row and wait until done
   *
   * Will insert new row linke insert(),
   *  but the function returns once internal state
   *  is EditingNewRow (case of success) or Visualizing (case of failure).
   *
   *  Note: wait will not freeze Qt's event loop.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool insertAndWait();

 public slots:

  /*! \brief Submit data
   *
   * If canWriteToDatabase flag is true,
   *  data will be stored to database,
   *  else they will only be keeped cached in model.
   *
   * Internally, submitTriggered() signal will be emitted.
   */
  void submit();

  /*! \brief Revert current record from model
   *
   * Internally, revertTriggered() signal will be emitted.
   */
  void revert();

  /*! \brief Insert a new row to model
   *
   * Internally, insertTriggered() signal will be emitted.
   */
  void insert();

  /*! \brief Remove a row from model
   *
   * Internally, removeTriggered() signal will be emitted.
   */
  void remove();

  /*! \brief Set last error
   *
   * Most of time, subclass uses directly pvLastError to set last error.
   *  In some (rare) cases, if needed, use this function to store a error.
   *  (This is f.ex. the case of mdtSqlTableWidget)
   */
  void setLastError(const mdtError & error)
  {
    pvLastError = error;
  }

 protected:

  /*! \brief Last error (for write access)
   */
  mdtError pvLastError;

  /*! \brief Get message handler
   *
   * Can return a Null pointer if no message handler was set (witch is acceptable case).
   */
  std::shared_ptr<mdtUiMessageHandler> messageHandler() { return pvMessageHandler; }

  /*! \brief Get internal proxy model
   */
  inline std::shared_ptr<mdtSortFilterProxyModel> proxyModel() { return pvProxyModel; }

  /*! \brief Beginn manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool beginTransaction();

  /*! \brief Rollback manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool rollbackTransaction();

  /*! \brief Commit manually a new transaction
   *
   * On error, false is returned and error is available with lastError() .
   */
  bool commitTransaction();

  /*! \brief Table model set event
   *
   * Cann be re-implemented in subclass
   *  if some things must be done once a table model was set.
   */
  virtual void modelSetEvent() {}

  /*! \brief Before current row change event
   *
   * Subclass can re-implement this function if some tasks must be done before current row changes.
   *  If current row change is not allowed, this function returns false.
   *
   * This default implementation does nothing and allways returns true.
   */
  virtual bool beforeCurrentRowChangeEvent()
  {
    return true;
  }

  /*! \brief Current row changed event
   *
   * Subclass can re-implement this method
   *  if some tasks should be done after current row changed.
   *
   * Note: don't wory about child controllers or other things,
   *  they are handled in this base class.
   */
  virtual void currentRowChangedEvent(int row) { Q_UNUSED(row) }

  /*! \brief Insert done event
   *
   * Subclass can re-implement this method
   *  if some tasks should be done after current row changed.
   *
   * Note: don't wory about child controllers or other things,
   *  they are handled in this base class.
   *
   * \param row Row (index) that was freshly insert. Note: row refers to proxyModel.
   */
  virtual void insertDoneEvent(int row) { Q_UNUSED(row) }

  /*! \brief Set current row - For internal use
   *
   * If row is > model's cached rowCount-1, data will be fetched
   *  in database. If request row could not be found, false is returned.
   *
   * Will call currentRowChangedEvent() , but will not update child controllers.
   *
   * \param row Row to witch to go. Must be in range [-1;rowCount()-1]
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   */
  bool setCurrentRowPv(int row);

  /*! \brief Submit data to model
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   *
   * Note: current row will be updated after successfull
   *  call of this method, calling then currentRowChangedEvent(),
   *  so subclass does not have to worry about this.
   */
  virtual bool submitToModel() = 0;

  /*! \brief Submit data from model to database
   */
  bool submitToDatabase();

  /*! \brief Submit current row to model
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   *
   * Note: current row will be updated after successfull
   *  call of this method, calling then currentRowChangedEvent().
   */
  //virtual bool doSubmit() = 0;

  /*! \brief Revert current row from model
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   */
  virtual bool doRevert() = 0;

  /*! \brief Submit new row to model
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   *
   * Note: current row will be updated after successfull
   *  call of this method, calling then currentRowChangedEvent().
   */
  //virtual bool doSubmitNewRow() = 0;

  /*! \brief Remove current row from model
   *
   * Subclass must implement this method.
   *  On problem, subclass should explain
   *  what goes wrong to the user and return false.
   */
  virtual bool doRemove() = 0;

 signals:

  /*! \brief Emitted when the entier widget enable state changes
   */
  void mainWidgetEnableStateChanged(bool enabled);

  /*! \brief Emitted when the entier widget enable state changes for a child widget
   */
  void childWidgetEnableStateChanged(bool enabled);

  /*! \brief Emitted when submit() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void submitEnabledStateChanged(bool enabled);

  /*! \brief Emitted when revert() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void revertEnabledStateChanged(bool enabled);

  /*! \brief Emitted when insert() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void insertEnabledStateChanged(bool enabled);

  /*! \brief Emitted when remove() function goes enabled/disabled
   *
   * This is usefull to keep GUI control coherent.
   *  For example, this is used in mdtSqlWindow to enable/disable actions.
   */
  void removeEnabledStateChanged(bool enabled);

  /*! \brief Emitted when select() or setFilter() was called
   */
  void selectTriggered();

  /*! \brief Emitted when submit() was called
   */
  void submitTriggered();

  /*! \brief Emitted when revert() was called
   */
  void revertTriggered();

  /*! \brief Emitted when insert() was called
   */
  void insertTriggered();

  /*! \brief Emitted when remove() was called
   */
  void removeTriggered();

  /*! \brief Emitted when a error occured
   */
  void errorOccured();

  /*! \brief Emitted when a operation succeed
   */
  void operationSucceed();

  /*! \brief Emitted when data was edited
   */
  void dataEdited();

  /*! \brief Emitted when Editing state was exited
   */
  void stateEditingExited();

  /*! \brief Emitted when Editing state was exited
   */
  void stateEditingNewRowExited();

 protected slots:

  /*! \brief Activity after Selecting state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateSelectingEntered();

  /*! \brief Activity after Visualizing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateVisualizingEntered();

  /*! \brief Activity after Visualizing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateVisualizingExited();

  /*! \brief Activity after Editing state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateEditingEntered();

  /*! \brief Activity after Editing state exited
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateEditingExited();

  /*! \brief Activity after Submitting state entered
   *
   * Will call checkBeforeSubmit() and, on success, doSubmit().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateSubmittingEntered();

  /*! \brief Activity after Reverting state entered
   *
   * Will call doRevert().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateRevertingEntered();

  /*! \brief Activity after Inserting state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateInsertingEntered();

  /*! \brief Activity after EditingNewRow state entered
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateEditingNewRowEntered();

  /*! \brief Activity after Editing state exited
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  virtual void onStateEditingNewRowExited();

  /*! \brief Activity after SubmittingNewRow state entered
   *
   * Will call checkBeforeSubmit() and, on success, doSubmitNewRow().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateSubmittingNewRowEntered();

  /*! \brief Activity after RevertingNewRow state entered
   *
   * Will call doRevertNewRow().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateRevertingNewRowEntered();

  /*! \brief Activity after Removing state entered
   *
   * Will call doRemove().
   *
   * This slot is called from internal state machine
   *  and should not be used directly.
   */
  void onStateRemovingEntered();

 private:

  /*! \brief Get a list of column indexes for given field names
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre Each given field name must exist in table
   */
  std::vector<int> columnList(const QStringList & fieldNames) const;

  /*! \brief Get the fisrt row for witch data in column matches matchData
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre column must exist in model.
   */
  int firstMatchingRow(int column, const QVariant & matchData);

  /*! \brief Get the fisrt row for witch data in column1 matches key1 and data in column2 matches key2
   *
   * Note: row is relative to sorted data model (proxyModel),
   *   not underlaying QSqlTableModel.
   *
   * \pre Table model must be set with setModel() or setTableName() begore calling this method.
   * \pre column must exist in model.
   */
  int firstMatchingRow(int column1, const QVariant & key1, int column2, const QVariant & key2);

  /*! \brief Setup and add child controller
   */
  bool setupAndAddChildController(std::shared_ptr<mdtAbstractSqlTableController> controller, const mdtSqlRelationInfo & relationInfo, QSqlDatabase db, const QString & userFriendlyChildTableName);

  /*! \brief Update child controllers after current row changed
   */
  void updateChildControllersAfterCurrentRowChanged();

  /*! \brief Update child controllers just after a insertion was done (in main table)
   */
  void updateChildControllersAfterInsert();

  /*! \brief Update child controllers just after submit was done (in main table)
   */
  bool updateChildControllersAfterSubmit();

  /*! \brief Update child controllers just after new row was submitted (in main table)
   */
  bool updateChildControllersAfterSubmitNewRow();

  /*! \brief Update cgild controllers just after revert was done (on main table)
   */
  void updateChildControllersAfterRevert();

  /*! \brief Update child controllers before removing a row (in main table)
   */
  bool updateChildControllersBeforeRemoveRow();

  /*! \brief Call checkBeforeSubmit() for each installed validators
   *
   * \sa addDataValidator()
   */
  bool checkBeforeSubmit();

  /*! \brief Wait until a operation completes
   *
   * Checks pvOperationComplete (don't forget to reset this flag before call)
   */
  void waitOperationComplete();

  /*! \brief Build the state machine
   */
  void buildStateMachine();

  Q_DISABLE_COPY(mdtAbstractSqlTableController);

  std::shared_ptr<QSqlTableModel> pvModel;
  std::shared_ptr<mdtFormatProxyModel> pvFormatProxyModel;
  std::shared_ptr<mdtSortFilterProxyModel> pvProxyModel;
  QList<std::shared_ptr<mdtSqlDataValidator> > pvDataValidators;
  std::shared_ptr<mdtUiMessageHandler> pvMessageHandler;
  QString pvUserFriendlyTableName;
  bool pvOperationComplete;
  QList<mdtAbstractSqlTableControllerContainer> pvChildControllerContainers;
  int pvCurrentRow;
  int pvBeforeInsertCurrentRow; // To go back by reverting new row
  bool pvCanWriteToDatabase;    // If true, submit will call model's submitAll(), else not
  // State machine members
  mdtState *pvStateSelecting;
  mdtState *pvStateVisualizing;
  mdtState *pvStateReverting;
  mdtState *pvStateEditing;
  mdtState *pvStateSubmitting;
  mdtState *pvStateInserting;
  mdtState *pvStateEditingNewRow;
  mdtState *pvStateRevertingNewRow;
  mdtState *pvStateSubmittingNewRow;
  mdtState *pvStateRemoving;
  mdtStateMachine pvStateMachine;
};

#endif  // #ifndef MDT_ABSTRACT_SQL_TABLE_CONTROLLER_H
