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
#ifndef MDT_ANALOG_IO_H
#define MDT_ANALOG_IO_H

#include "mdtAbstractIo.h"
#include <QVariant>
#include <memory>

///class mdtBinaryDoubleConverter;
#include "mdtBinaryDoubleConverter.h"

/*! \brief Representation of a analog I/O
 *
 * On wide range of automation applications,
 *  it is usefull to display a I/O to the user.
 *  For example, a analog I/O module, with a 8 bit
 *  encoding could be displayed with this class
 *  togther with mdtAnalogInWidget or mdtAnalogOutWidget.
 *
 * \todo Because the class must handle storage + UI interactions,
 *       and because it can be used for a varety of cases,
 *       conversions should be done by another class.
 *       F.ex., if device talks "Itegral binary" , such as Waogo 750 I/O modules,
 *       conversion must be done and stored here.
 *       With a multimeter (f.ex. Agilent U3606A), witch talks ASCII,
 *       conversion is allready done in frame codec, and any value must be stored here AS IS.
 */
class mdtAnalogIo : public mdtAbstractIo
{
 Q_OBJECT

 public:

  /*! \brief Constructor
   */
  mdtAnalogIo(QObject *parent = 0);

  /*! \brief Destructor
   */
  ~mdtAnalogIo();

  /*! \brief Set the unit (V, A, °C, ...)
   */
  void setUnit(const QString &unit);

  /*! \brief Get unit
   */
  QString unit() const;

  /*! \brief Specify the range of values and conversion parameters
   *
   * Store the new range and set the value to the minimum.
   *  The valueChanged() signal is emitted.
   *
   * This method will also enable conversions.
   *  See mdtBinaryDoubleConverter class for details about conversion setup.
   *
   * \param min Minimum value to display (f.ex. 0V, or 4mA)
   * \param max Maximum value to display (f.ex. 10V, or 20mA)
   * \param intValueBitsCount Number of bits used for the value, including sign bit.
   * \param intValueLsbIndex Index of first bit to use for value extraction.
   * \param intValueSigned If true, decoded value is considered as signed value, i.e. the most significant bit is the sign.
   * \param scaleFromMinToMax If true, given value INT will be scaled from min to max regarding LSB.
   *                           LSB is determined from intValueBitsCount .
   * \param conversionFactor This factor is applied as follow (min, max scaling is ignored in this example):
   *                          - ValueDouble = valueInt * LSB * conversionFactor
   *                          - ValueInt = valueDouble / (LSB * conversionFactor)<br>
   *                          LSB is determined from intValueBitsCount .
   *
   * \return true on success. Some checks are done, and if one fails, false is returned and
   *          the previous settingsare used for conversion.
   *
   * \pre max must be > min
   *
   * Note for UI developpers:
   *  - The signal rangeChangedForUi() is emited
   *  - The signal valueChangedForUi() is emited
   */
  void setRange(double min, double max, int intValueBitsCount, int intValueLsbIndex = 0, bool intValueSigned = false, bool scaleFromMinToMax = true, double conversionFactor = 1.0);

  /*! \brief Get the minimum value of the range
   */
  double minimum() const;

  /*! \brief Get the maximum value of the range
   */
  double maximum() const;

  /*! \brief Set the integer value
   *
   * Will calculate the real value depending on resolution and update display.
   *  The resolution is set with setRange() (intValueBitsCount parameter)
   *
   * The valueChanged() signal is emitted.
   *
   * \param value The value to store
   * \param isValid The validity flag. This flag is later avaliable with mdtAbstractIo::hasValidData()
   * \param emitValueChanged If true, valueChanged() will not be emitted
   *
   * Note for UI developpers:
   *  - The signal valueChangedForUi() is emited
   */
  void setValueInt(int value, bool isValid, bool emitValueChanged);

 public slots:

  /*! \brief Set the value to update display
   *
   * \sa mdtAbstractIo::setValue(const mdtValue&, bool)
   */
  void setValue(const mdtValue &value, bool emitValueChanged = false);

 signals:

  /*
   * These signals are emited every time
   *  a member is set with a setter method.
   * Usefull to update the UI (should not be used for other purpose)
   */
  void unitChangedForUi(const QString &unit);
  void rangeChangedForUi(double min, double max);

 private slots:

  // Used from UI to update internal value.
  //  The valueChangedForUi() signal will not be emitted with this call.
  void setValueFromUi(double value);

 private:

  /*! \brief Store double part, calculate integer part and store it
   *
   * If given value is different than stored one:
   *  - Conversion and storage are done
   *  - valueChangedForUi() is emitted
   *  - If emitValueChanged is true, valueChanged() is emitted
   */
  void setValueFromDouble(const mdtValue &value, bool emitValueChanged);

  /*! \brief Store integer part, calculate double part and store it
   *
   * If given value is different than stored one:
   *  - Conversion and storage are done
   *  - valueChangedForUi() is emitted
   *  - If emitValueChanged is true, valueChanged() is emitted
   */
  void setValueFromInt(const mdtValue &value, bool emitValueChanged);

  Q_DISABLE_COPY(mdtAnalogIo);

  QString pvUnit;
  double pvMinimum;
  double pvMaximum;
  ///double pvStepQ;             // Quantification step, it is also the factor used for Int (encoded) -> Floating (analog value) conversion (D/A conversion)
  ///double pvStepAD;            // Factor used for Floating (analog value) -> Int (encoded) conversion (A/D conversion)
  // Members used for integer <-> float value conversion
  /**
  int pvIntValueLsbIndex;     // Index of least significant bit, used in setValueInt()
  int pvIntValueLsbIndexEnc;  // Index of least significant bit, used in valueInt()
  int pvIntValueMask;         // Mask to extract the correct bits count, used in setValueInt()
  int pvIntValueMaskEnc;      // Mask to extract the correct bits count, used in valueInt()
  int pvIntValueSignMask;
  bool pvIntValueSigned;
  bool pvScaleFromMinToMax;
  */
  std::unique_ptr<mdtBinaryDoubleConverter<int> > pvConverter;
};

#endif  // #ifndef MDT_ANALOG_IO_H
