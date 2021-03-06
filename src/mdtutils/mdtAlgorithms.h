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
#ifndef MDT_ALGORITHMS_H
#define MDT_ALGORITHMS_H

#include <QtAlgorithms>
#include <QChar>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <vector>

/*! \brief Some usefull little (unefficient) algoritms
 */
namespace mdtAlgorithms
{
  /*! \brief Numeric range
   */
  struct NumericRangeDouble
  {
    /*! \brief Construct a invalid range
     */
    NumericRangeDouble();

    /*! \brief Construct a range that represents all values from min to max
     */
    NumericRangeDouble(double min, double max, bool includeMin = true, bool includeMax = true);

    /*! \brief Check if range is valid
     *
     * A range is valid if bottom is < top
     */
    inline bool isValid() const
    {
      return (bottom < top);
    }

    /*! \brief Bottom of the range
     */
    double bottom;

    /*! \brief Top of the range
     */
    double top;

    /*! \brief bottom is part of range (or not)
     */
    bool includeBottom;

    /*! \brief top is part of range (or not)
     */
    bool includeTop;
  };

  /*! \brief Check if x is in range min;max
   *
   * If both includeBottom and includeTop are true,
   *  true is returned if x is in [bottom;top].
   *
   * If only includeBottom is true,
   *  true is returned if x is in [bottom;top[.
   *
   * If only includeTop is true,
   *  true is returned if x is in ]bottom;top].
   *
   * For the last case,
   *  true is returned if x is in ]bottom;top[.
   *
   * \pre bottom must be < top
   */
  bool isInRange(double x, double bottom, double top, bool includeBottom, bool includeTop);

  /*! \brief Check if x is in range
   *
   * \pre range must be valid
   */
  inline bool isInRange(double x, const NumericRangeDouble & range)
  {
    Q_ASSERT(range.isValid());
    return isInRange(x, range.bottom, range.top, range.includeBottom, range.includeTop);
  }

  /*! \brief Get the range in witch x is
   *
   * If x is in no range in ranges,
   *  a invalid range is returned.
   */
  template<typename RangeType>
  RangeType rangeOf(double x, const std::vector<RangeType> & ranges)
  {
    for(auto & r : ranges){
      if(isInRange(x, r)){
        return r;
      }
    }
    return RangeType();
  }

  /*! \brief Sort a string list using natural sort
   *
   * Usefull for directory entry list. By default, QDir sorts the entries
   * at alphabetic order. For exemple, if we have ttyS0, ttyS1, ttyS2, ... , ttyS10 ,
   * the entries will be sorted: ttyS0, ttyS1, ttyS10 , ttyS2, ...
   * With this function, the result will be: ttyS0, ttyS1, ttyS2, ..., ttyS10, ...
   *
   * \param list The list that will be sorted (will be altered)
   * \return The sorted list
   */
  QStringList naturalSort(QStringList &list, Qt::CaseSensitivity caseSensitivity);

  /*! \brief Compare 2 strings using natural sort
   *
   * This method chops the input str1 and str2 into pieces of digits and non-digits
   *  (a1.05 becomes |a|1|.|05) and compare these pieces of str1 and str2 to each other
   *  (first with first, second with second, ...).
   *
   * This function is based on KDE KStringHandler::naturalCompare():
   *  http://api.kde.org/4.10-api/kdelibs-apidocs/kdecore/html/kstringhandler_8cpp_source.html
   *
   * KStringHandler itself is based on the natural sort order code from Martin Pool:
   *  http://sourcefrog.net/projects/natsort/
   *
   * Please note that this version is not the original one.
   *  Unexpected behaviours cannot be due to KDE or Martin Pool.
   *
   * \return True if str1 < str2.
   */
  bool naturalCompareLessThan(const QString &str1, const QString &str2, Qt::CaseSensitivity caseSensitivity);

  /*! \brief Compare 2 strings using natural sort - case sensitive version
   */
  bool naturalCompareLessThanCaseSensitive(const QString &str1, const QString &str2);

  /*! \brief Compare 2 strings using natural sort - case insensitive version
   */
  bool naturalCompareLessThanCaseInsensitive(const QString &str1, const QString &str2);

  /*! \brief Convert a hexadecimal formated string to numerical bytes array
   * 
   * Hexadecimal string is, for example, 2C 12 5F ,
   *  witch will be converted to 44,18,95 numeric byte array.
   * 
   * \pre hexStr must contain multiple of 2 valid chars. F.ex: 2C 12 5 is not valid.
   * \pre hexStr must contain valid chars (0-9 , a-f, A-F). F.ex: 2Z is not valid.
   */
  QByteArray hexStringToByteArray(const QString &hexStr);

  /*! \brief Convert a byte array to a hexadecimal string.
   * 
   * For example, byte array with 44,18,95 numerical values
   *  will generate a string with "2C 12 5F".
   */
  QString byteArrayToHexString(const QByteArray &byteArray);

  /*! \brief Get unprotected string
   *
   * It can happen that a protected string, f.ex. "Hello", must
   *  be extracted. Additionnally, escape chars can be inside.
   * This is a helper function used by splitString().
   *
   * For example, if data protection is " and escape char is \ ,
   *  "A\"BC\"D" will return A"BC"D.
   * If escape char is alone, nothing will happen:
   *  "A\BCD" will return A\BCD.
   * If data proection not exists, a empty string will be returned.
   * Note that exactly one protected section will be parsed. For example,
   *  "AB"CD"EF" will return AB, CD"EF" will simply be ignored.
   *
   * \param str The input string to parse
   * \param dataProtection Data protection (typical: " )
   * \param escapeChar Escape char (typical: \ )
   * \param strEndOffset If not Null, the offset (of first char) of closing dataProtection,
   *                      before removing escape chars, is stored to this parameter.
   *                      This is used by splitString(). Some examples with " as dataProtection and \ as escapeChar:
   *                       - For "A\"BC\"D" : value will be 9
   *                       - For "A\BCD" : value will be 6
   *                       - For "A\BCD : value will be -1 (no closing " found)
   *                       - For A : value will be -1 (no " found)
   * \param strStartOffset If > -1, parsing will begin a this position in str
   */
  QString unprotectedString(const QString &str, const QString &dataProtection, const QChar &escapeChar, int *strEndOffset = 0, int strStartOffset = -1);

  /*! \brief Split a string reagrding a separator and data protection
   *
   * QString has a totally functionnal split method, but it does not
   *  support data proection.
   * For example, if we have to split a string: "123";"AB;C" witch uses
   *  ; char as separator and " as protection, we must have a list
   *  with 2 items: 123 and AB;C.
   *
   * Some details:
   *  - If str is empty, a empty list is returned
   *  - Empty records (data around separator) are added to list
   *
   * \param str The input string to parse
   * \param separator Separator to use (typical: ; )
   * \param dataProtection Data protection (typical: " )
   * \param escapeChar Escape char (typical: \ ). Note: has only effect to escape dataProtection.
   * \return The list of found fields
   */
  QStringList splitString(const QString &str, const QString &separator, const QString &dataProtection, const QChar &escapeChar = QChar());

  /*! \brief Get the string of longest line in given string
   */
  QString longestLineInString(const QString & str, const QString & sep = "\n");

  /*! \brief Generate a random string
   *
   * \param length Length of the string to generate
   * \note Internally qrand() is used, so this function is thread safe.
   *       qsrand() should be called once before calling this function.
   */
  QString generateString(int length);

  /*! \brief Generate a random string
   *
   * \param length Length of the string to generate
   * \param excludeList List of strings that must be excluded.
   *                    As long as the generated string matches one of these strings,
   *                    a new one is generated.
   * \note Internally qrand() is used, so this function is thread safe.
   *       qsrand() should be called once before calling this function.
   */
  QString generateString(int length, const QStringList excludeList);
}

#endif  // #ifndef MDT_ALGORITHMS_H
