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
#ifndef MDT_ABSTRACT_IO_WIDGET_H
#define MDT_ABSTRACT_IO_WIDGET_H

#include <QWidget>

class QLabel;
class QPushButton;
class QMessageBox;
class mdtAbstractIo;

/*! \brief Base class for I/O widgets
 */
class mdtAbstractIoWidget : public QWidget
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   *
   * Note for subclass:
   *  Some widgets will be created in this consructor:
   *   - lbLabel: a QLabel wich display the short label of I/O and the label of I/O as tool tip
   *   - pbDetails: a QPushButton that will call a dialog wich displays details of I/O
   *  The subclass must place these labels into a layout.
   */
  mdtAbstractIoWidget(QWidget *parent = 0);

  virtual ~mdtAbstractIoWidget();

 protected:

   /*! \brief Set the I/O object
   *
   * Make all needed connections with the I/O
   *  base object, so that widget is allways updated.
   *
   * Used only by subclass.
   *
   * \pre io must be a valid pointer.
   */
  void setIo(mdtAbstractIo *io);

  // Widgets that are shared with subclass
  QLabel *lbLabel;
  QPushButton *pbDetails;

 private slots:

  // Display the message box with all I/O informations
  void showIoInformations();

  // Used to update GUI from mdtAbstractIo object.
  void setAddress(int addressRead, int addressWrite);
  void setLabelShort(const QString & text);
  void setLabel(const QString & text);
  void setDetails(const QString & text);

 private:

  Q_DISABLE_COPY(mdtAbstractIoWidget);

  int pvAddressRead;
  int pvAddressWrite;
  QMessageBox *pvMessageBox;
};

#endif  // #ifndef
