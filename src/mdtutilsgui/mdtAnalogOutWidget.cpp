/****************************************************************************
 **
 ** Copyright (C) 2011-2012 Philippe Steinmann.
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
#include "mdtAnalogOutWidget.h"
#include "mdtAnalogIo.h"
#include <qwt_slider.h>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <cmath>
#include <float.h>

mdtAnalogOutWidget::mdtAnalogOutWidget(QWidget *parent)
 : mdtAbstractIoWidget(parent)
{
  QGridLayout *l = new QGridLayout;

  // Setup GUI
  l->addWidget(lbLabel, 0, 0);
  slValue = new QwtSlider(this, Qt::Vertical, QwtSlider::LeftScale);
  l->addWidget(slValue, 1, 0);
  sbValue = new QDoubleSpinBox;
  l->addWidget(sbValue, 2, 0);
  l->addWidget(pbDetails, 3, 0);
  setLayout(l);
  // Synchronize spinbox and slider
  connect(slValue, SIGNAL(valueChanged(double)), sbValue, SLOT(setValue(double)));
  connect(sbValue, SIGNAL(valueChanged(double)), slValue, SLOT(setValue(double)));
}

mdtAnalogOutWidget::~mdtAnalogOutWidget()
{
}

void mdtAnalogOutWidget::setIo(mdtAnalogIo *io)
{
  Q_ASSERT(io != 0);

  // Base Signals/slots connections
  mdtAbstractIoWidget::setIo(io);
  // Signals/slots from io to widget
  connect(io, SIGNAL(unitChangedForUi(const QString&)), this, SLOT(setUnit(const QString&)));
  connect(io, SIGNAL(rangeChangedForUi(double, double)), this, SLOT(setRange(double, double)));
  connect(io, SIGNAL(valueChangedForUi(double)), this, SLOT(setValue(double)));
  // Signals/slots from widget to io
  connect(slValue, SIGNAL(valueChanged(double)), io, SLOT(setValueFromUi(double)));
}

void mdtAnalogOutWidget::setUnit(const QString &unit)
{
  pvUnit = unit;
}

void mdtAnalogOutWidget::setRange(double min, double max)
{
  slValue->setRange(min, max);
  sbValue->setRange(min, max);
}

void mdtAnalogOutWidget::setValue(double value)
{
  sbValue->setValue(value);
}
