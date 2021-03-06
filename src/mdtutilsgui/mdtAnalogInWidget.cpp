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
#include "mdtAnalogInWidget.h"
#include "mdtAnalogIo.h"
#include <qwt_thermo.h>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QFontMetrics>

mdtAnalogInWidget::mdtAnalogInWidget(QWidget *parent)
 : mdtAbstractIoWidget(parent)
{
  QGridLayout *l = new QGridLayout;

  // Setup GUI
  l->addWidget(lbLabel, 0, 0, 1, 2);
  thValue = new QwtThermo;
  l->addWidget(thValue, 1, 0, 1, 2);
  lbValue = new QLabel;
  lbValue->setAlignment(Qt::AlignLeft);
  l->addWidget(lbValue, 2, 0, 1, 1);
  lbUnit = new QLabel;
  l->addWidget(lbUnit, 2, 1, 1, 1);
  l->addWidget(pbDetails, 3, 0, 1, 2);
  setLayout(l);
}

mdtAnalogInWidget::~mdtAnalogInWidget()
{
}

void mdtAnalogInWidget::setIo(mdtAnalogIo *io)
{
  Q_ASSERT(io != 0);

  // Base Signals/slots connections
  mdtAbstractIoWidget::setIo(io);
  // Signals/slots from io to widget
  connect(io, SIGNAL(unitChangedForUi(const QString&)), this, SLOT(setUnit(const QString&)));
  connect(io, SIGNAL(rangeChangedForUi(double, double)), this, SLOT(setRange(double, double)));
  connect(io, SIGNAL(valueChangedForUi(const mdtValue&)), this, SLOT(setValue(const mdtValue&)));
  connect(io, SIGNAL(enabledStateChangedForUi(bool)), this, SLOT(setEnabled(bool)));
  // Set initial data
  setUnit(io->unit());
  setRange(io->minimum(), io->maximum());
  // Set a minimum width for lbValue
  lbValue->setMinimumWidth(lbValue->fontMetrics().width("-0.000e-00 "));
  setValue(io->value());
}

void mdtAnalogInWidget::setUnit(const QString &unit)
{
  pvUnit = unit;
  lbUnit->setText(pvUnit);
}

void mdtAnalogInWidget::setRange(double min, double max)
{
  thValue->setScale(min, max);
}

void mdtAnalogInWidget::setValue(const mdtValue &value)
{
  QString sValue;

  if(value.isValid()){
    Q_ASSERT(value.hasValueDouble());
    thValue->setValue(value.valueDouble());
    if(value.isMinusOl()){
      lbValue->setText("-OL");
    }else if(value.isPlusOl()){
      lbValue->setText("+OL");
    }else{
      sValue.setNum(value.valueDouble(), 'g', 4);
      lbValue->setText(sValue);
    }
  }else{
    thValue->setValue(thValue->lowerBound());
    lbValue->setText("??");
  }
}

void mdtAnalogInWidget::setEnabled(bool enabled)
{
  lbValue->setEnabled(enabled);
  thValue->setEnabled(enabled);
}
