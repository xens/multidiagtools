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
#include "mdtFieldMapField.h"
#include "mdtError.h"

QStringList mdtFieldMapField::spvIllegalStrings = QStringList();

mdtFieldMapField::mdtFieldMapField()
{
  if(spvIllegalStrings.isEmpty()){
    spvIllegalStrings.append(" ");
    spvIllegalStrings.append("?");
    spvIllegalStrings.append("+");
    spvIllegalStrings.append("-");
    spvIllegalStrings.append("*");
    spvIllegalStrings.append("/");
    spvIllegalStrings.append("\\");
    spvIllegalStrings.append("'");
    spvIllegalStrings.append("<");
    spvIllegalStrings.append(">");
    spvIllegalStrings.append("=");
    spvIllegalStrings.append("~");
    spvIllegalStrings.append("!");
    spvIllegalStrings.append("`");
    spvIllegalStrings.append("$");
    spvIllegalStrings.append("(");
    spvIllegalStrings.append(")");
    spvIllegalStrings.append("[");
    spvIllegalStrings.append("]");
    spvIllegalStrings.append("{");
    spvIllegalStrings.append("}");
    spvIllegalStrings.append(".");
    spvIllegalStrings.append(",");
    spvIllegalStrings.append(":");
    spvIllegalStrings.append(";");
    spvIllegalStrings.append("&");
    spvIllegalStrings.append("|");
    spvIllegalStrings.append("#");
    spvIllegalStrings.append("@");
    spvIllegalStrings.append("%");
    spvIllegalStrings.append("\"");
  }
}

void mdtFieldMapField::setIndex(int value) 
{
  pvIndex = value;
}

int mdtFieldMapField::index() const 
{
  return pvIndex;
}

void mdtFieldMapField::setDisplayText(const QString & text) 
{
  pvDisplayText = text;
}

QString mdtFieldMapField::displayText() const 
{
  return pvDisplayText;
}

void mdtFieldMapField::setName(const QString & name) 
{
  pvSqlField.setName(name);
}

QString mdtFieldMapField::name() const
{
  return pvSqlField.name();
}

void mdtFieldMapField::setSqlField(const QSqlField & field)
{
  pvSqlField = field;
}

QSqlField mdtFieldMapField::sqlField() const
{
  return pvSqlField;
}

QSqlField &mdtFieldMapField::sqlField()
{
  return pvSqlField;
}

bool mdtFieldMapField::strContainsIllegalString(const QString &str)
{
  int i;

  for(i = 0; i < spvIllegalStrings.size(); ++i){
    if(str.contains(spvIllegalStrings.at(i))){
      return true;
    }
  }

  return false;
}

QString mdtFieldMapField::getFieldName(const QString &name, const QString &replaceBy)
{
  int i;
  QString newName;
  QString replace = replaceBy;

  if(strContainsIllegalString(replaceBy)){
    replace = "_";
    mdtError e("Replacement string '" + replaceBy + "' contains illegal string. Using '_' instead.", mdtError::Warning);
    MDT_ERROR_SET_SRC(e, "mdtFieldMapField");
    e.commit();
  }
  newName = name;
  for(i = 0; i < spvIllegalStrings.size(); ++i){
    newName.replace(spvIllegalStrings.at(i), replace);
  }

  return newName;
}
