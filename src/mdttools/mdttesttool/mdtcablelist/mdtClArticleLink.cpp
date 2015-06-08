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
#include "mdtClArticleLink.h"

mdtClArticleLink::mdtClArticleLink(QObject *parent, QSqlDatabase db)
 : mdtTtBase(parent, db)
{
}

mdtClArticleLink::mdtClArticleLink(QSqlDatabase db)
 : mdtClArticleLink(nullptr, db)
{
}

bool mdtClArticleLink::addLink(const mdtClArticleLinkData &data)
{
  mdtSqlRecord record;

  if(!record.addAllFields("ArticleLink_tbl", database())){
    pvLastError = record.lastError();
    return false;
  }
  fillRecord(record, data);

  return addRecord(record, "ArticleLink_tbl");
}

void mdtClArticleLink::fillRecord(mdtSqlRecord &record, const mdtClArticleLinkData &data)
{
  Q_ASSERT(!record.isEmpty());

  auto key = data.keyData();

  record.clearValues();
  record.setValue("ArticleConnectionStart_Id_FK", key.connectionStartFk.id);
  record.setValue("ArticleConnectionEnd_Id_FK", key.connectionEndFk.id);
  record.setValue("LinkType_Code_FK", key.linkTypeFk.code);
  record.setValue("LinkDirection_Code_FK", key.linkDirectionFk.code);
  record.setValue("Identification", data.indetification);
  record.setValue("SinceVersion", data.sinceVersion);
  record.setValue("Modification", data.modification);
  record.setValue("Resistance", data.resistance);
}
