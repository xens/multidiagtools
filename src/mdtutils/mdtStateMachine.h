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
#ifndef MDT_STATE_MACHINE_H
#define MDT_STATE_MACHINE_H

#include <QStateMachine>
#include <QString>
#include <QVector>

class mdtState;
class QTimer;

/*! \brief Extend Qt's QStateMachine
 *
 * QStateMachine provides all that is needed to build a state machine.
 *  But, for some application, some other things can be usefull:
 *   - Get current state
 *   - Wait until a state was reached
 *   - Notify only some states to the (G)UI
 *
 * Usually, a system based on a state machine is only event driven,
 *  and QStateMachine was build this way.
 *  But, for some application, we need to be able to wait
 *  until a state is reached, without breaking application's event loop, before we can continue.
 */
class mdtStateMachine : public QStateMachine
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   *
   * Current state will be set to -1 (invalid).
   */
  mdtStateMachine(QObject *parent = 0);

  /*! \brief Destructor
   */
  virtual ~mdtStateMachine();

  /*! \brief Start state machine
   *
   * \param waitStarted If true, call will return only when state machine has started (will not break event loop)
   */
  void start(bool waitStarted);

  /*! \brief Stop state machine
   *
   * \param waitStopped If true, call will return only when state machine is stopped (will not break event loop)
   */
  void stop(bool waitStopped);

  /*! \brief Get current state
   */
  int currentState() const;

  /*! \brief Wait until a state is reached
   *
   * This wait method will let the application's event loop continue working,
   *  so it will not freeze GUI.
   *
   * \param state ID if state on witch to wait.
   * \param timeout Maximum time to wait [ms].
   *                 Value < 0 means a infinite timeout.
   * \return True on success, false on timeout.
   */
  bool waitOnState(int state, int timeout = -1);

  /*! \brief Wait until a state is reached
   *
   * This wait method will let the application's event loop continue working,
   *  so it will not freeze GUI.
   *
   * \param states Wait of one of state in the list. If one is reached before timeout, this method returns true
   * \param timeout Maximum time to wait [ms].
   *                 Value < 0 means a infinite timeout.
   * \return True on success, false on timeout.
   */
  bool waitOnOneState(const QVector<int> & states, int timeout = -1);

  /*! \brief Notify current state
   *
   * Will emit stateChanged() and stateChangedForUi() (if required by state).
   *
   * In normal way, signals are emitted when state changes.
   *  In some situation, it can be usefull to call this method explicitly
   *  (see mdtPortManager::notifyCurrentState() for a example).
   */
  void notifyCurrentState();

 signals:

  /*! \brief Emitted each time the state changed
   */
  void stateChanged(int id);

  /*! \brief Emitted only if a state that must be notified is entered
   *
   * \sa mdtState::notifyEnteredToUi()
   */
  void stateChangedForUi(int id, const QString & text, int ledColorId, bool ledIsOn);

 private slots:

  /*! \brief Called by mdtState when entered
   */
  void onStateEntered(mdtState *state);

  /*! \brief Set the pvTimeoutOccured flag.
   */
  void onWaitTimeout();

  /*! \brief Set started flag
   */
  void setStartedFlag();

  /*! \brief Set stopped flag
   */
  void setStoppedFlag();

 private:

  Q_DISABLE_COPY(mdtStateMachine);

  QTimer *pvTimeoutTimer;
  mdtState *pvCurrentState;
  bool pvTimeoutOccured;
  bool pvStarted;
  bool pvStopped;
};

#endif  // #ifndef MDT_STATE_MACHINE_H
