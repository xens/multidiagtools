/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
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
#include "mdtPortConfigWidget.h"
#include "mdtFrame.h"
#include <QVariant>
#include <QByteArray>

//#include <QDebug>

mdtPortConfigWidget::mdtPortConfigWidget(QWidget *parent)
 : QWidget(parent)
{
  setupUi(this);
}

void mdtPortConfigWidget::displayConfig(mdtPortConfig &config)
{
  int index;
  QByteArray ba;

  // Frame type
  if(cbFrameType->count() < 1){
    cbFrameType->addItem(tr("Binary raw") , mdtFrame::FT_RAW);
    cbFrameType->addItem(tr("Binary timeout protocol") , mdtFrame::FT_RAW_TOP);
    cbFrameType->addItem(tr("ASCII") , mdtFrame::FT_ASCII);
    cbFrameType->addItem(tr("Modbus TCP") , mdtFrame::FT_MODBUS_TCP);
    cbFrameType->addItem(tr("USBTMC") , mdtFrame::FT_USBTMC);
  }
  index = cbFrameType->findData(config.frameType());
  if(index >= 0){
    cbFrameType->setCurrentIndex(index);
  }
  // End of frame sequence
  if(cbEndOfFrameSeq->count() < 1){
    cbEndOfFrameSeq->addItem("", QByteArray());
    cbEndOfFrameSeq->addItem("*", QByteArray("*"));
    cbEndOfFrameSeq->addItem("$", QByteArray("$"));
    ba.clear();
    ba.append((char)0x04);
    cbEndOfFrameSeq->addItem("EOT (0x04)", ba);
    ba.clear();
    ba.append((char)0x0A);
    cbEndOfFrameSeq->addItem("LF (0x0A, \\n)", ba);
    ba.clear();
    ba.append((char)0x0D);
    cbEndOfFrameSeq->addItem("CR (0x0D, \\r)", ba);
    ba.clear();
    ba.append((char)0x17);
    cbEndOfFrameSeq->addItem("ETB (0x17)", ba);

  }
  index = cbEndOfFrameSeq->findData(config.endOfFrameSeq());
  if(index >= 0){
    cbEndOfFrameSeq->setCurrentIndex(index);
  }
  // Read options
  sbReadFrameSize->setValue(config.readFrameSize());
  sbReadQueueSize->setValue(config.readQueueSize());
  sbReadTimeout->setValue(config.readTimeout());
  cbUseReadTimeoutProtocol->setChecked(config.useReadTimeoutProtocol());
  // Write options
  sbWriteFrameSize->setValue(config.writeFrameSize());
  sbWriteQueueSize->setValue(config.writeQueueSize());
  sbWriteTimeout->setValue(config.writeTimeout());
  sbWriteInterbyteWaitTime->setValue(config.writeInterbyteTime());
  cbBytePerByteWrite->setChecked(config.bytePerByteWrite());
  sbWriteInterframeTime->setValue(config.writeInterframeTime());
}

void mdtPortConfigWidget::updateConfig(mdtPortConfig &config)
{
  int index;

  // Frame type
  index = cbFrameType->currentIndex();
  if(index >= 0){
    switch(cbFrameType->itemData(index).toInt()){
      case mdtFrame::FT_RAW:
        config.setFrameType(mdtFrame::FT_RAW);
        break;
      case mdtFrame::FT_RAW_TOP:
        config.setFrameType(mdtFrame::FT_RAW_TOP);
        break;
      case mdtFrame::FT_ASCII:
        config.setFrameType(mdtFrame::FT_ASCII);
        break;
      case mdtFrame::FT_MODBUS_TCP:
        config.setFrameType(mdtFrame::FT_MODBUS_TCP);
        break;
    }
  }
  // End of frame sequence - Only for ASCII frame type
  if(config.frameType() == mdtFrame::FT_ASCII){
    index = cbEndOfFrameSeq->currentIndex();
    if(index > -1){
      config.setEndOfFrameSeq(cbEndOfFrameSeq->itemData(index).toByteArray());
    }
  }else{
    config.setEndOfFrameSeq("");
  }
  // Read options
  config.setReadFrameSize(sbReadFrameSize->value());
  config.setReadQueueSize(sbReadQueueSize->value());
  config.setReadTimeout(sbReadTimeout->value());
  config.setUseReadTimeoutProtocol(cbUseReadTimeoutProtocol->isChecked());
  // Write options
  config.setWriteFrameSize(sbWriteFrameSize->value());
  config.setWriteQueueSize(sbWriteQueueSize->value());
  config.setWriteTimeout(sbWriteTimeout->value());
  config.setBytePerByteWrite(cbBytePerByteWrite->isChecked(), sbWriteInterbyteWaitTime->value());
  config.setWriteInterframeTime(sbWriteInterframeTime->value());

  // Display real applied setup
  displayConfig(config);
}

void mdtPortConfigWidget::on_cbFrameType_currentIndexChanged(int index)
{
  // Make the end of frame sequence selection coherant to frame type
  if(cbFrameType->itemData(index) != mdtFrame::FT_ASCII){
    cbEndOfFrameSeq->setEnabled(false);
  }else{
    cbEndOfFrameSeq->setEnabled(true);
  }
  // If frame type is binary timeout protocol, activate timeout protocl
  if(cbFrameType->itemData(index) == mdtFrame::FT_RAW_TOP){
    cbUseReadTimeoutProtocol->setChecked(true);
  }else{
    cbUseReadTimeoutProtocol->setChecked(false);
  }
}
