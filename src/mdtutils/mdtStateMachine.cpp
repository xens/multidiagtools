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
#include "mdtStateMachine.h"
#include "mdtState.h"
#include <QTimer>
#include <QCoreApplication>
#include <QEventLoop>

mdtStateMachine::mdtStateMachine(QObject *parent)
 : QStateMachine(parent)
{
  pvTimeoutTimer = new QTimer(this);
  pvTimeoutTimer->setSingleShot(true);
  pvTimeoutOccured = false;
  connect(pvTimeoutTimer, SIGNAL(timeout()), this, SLOT(onWaitTimeout()));
  connect(this, SIGNAL(started()), this, SLOT(setStartedFlag()));
  connect(this, SIGNAL(stopped()), this, SLOT(setStoppedFlag()));
  pvCurrentState = 0;
}

mdtStateMachine::~mdtStateMachine()
{
}

void mdtStateMachine::start(bool waitStarted)
{
  pvStarted = false;
  QStateMachine::start();
  if(waitStarted){
    while(!pvStarted){
      QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
    }
  }
}

void mdtStateMachine::stop(bool waitStopped)
{
  pvStopped = false;
  QStateMachine::stop();
  if(waitStopped){
    while(!pvStopped){
      QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
    }
  }
}

int mdtStateMachine::currentState() const
{
  if(pvCurrentState == 0){
    return -1;
  }
  return pvCurrentState->id();
}

bool mdtStateMachine::waitOnState(int state, int timeout)
{
  pvTimeoutOccured = false;
  if(currentState() == state){
    return true;
  }
  if(timeout > 0){
    pvTimeoutTimer->start(timeout);
  }
  while(currentState() != state){
    if(pvTimeoutOccured){
      return false;
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
  pvTimeoutTimer->stop();

  return true;
}

bool mdtStateMachine::waitOnOneState(const QVector<int> & states, int timeout)
{
  pvTimeoutOccured = false;
  if(states.contains(currentState())){
    return true;
  }
  if(timeout > 0){
    pvTimeoutTimer->start(timeout);
  }
  while(states.contains(currentState())){
    if(pvTimeoutOccured){
      return false;
    }
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
  }
  pvTimeoutTimer->stop();

  return true;
}

void mdtStateMachine::notifyCurrentState()
{
  if(pvCurrentState == 0){
    emit stateChanged(-1);
  }else{
    emit stateChanged(pvCurrentState->id());
    if(pvCurrentState->notifyEnteredToUi()){
      emit stateChangedForUi(pvCurrentState->id(), pvCurrentState->text(), pvCurrentState->ledColorId(), pvCurrentState->ledIsOn());
    }
  }
}

void mdtStateMachine::onStateEntered(mdtState *state)
{
  Q_ASSERT(state != 0);

  pvCurrentState = state;
  notifyCurrentState();
}

void mdtStateMachine::onWaitTimeout()
{
  pvTimeoutOccured = true;
}

void mdtStateMachine::setStartedFlag()
{
  pvStarted = true;
}

void mdtStateMachine::setStoppedFlag()
{
  pvStopped = true;
}
