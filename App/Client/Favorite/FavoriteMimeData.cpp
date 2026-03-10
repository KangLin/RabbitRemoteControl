// Author: Kang Lin <kl222@126.com>

#include "FavoriteMimeData.h"

CFavoriteMimeData::CFavoriteMimeData(QObject *parent) : QMimeData()
{
}

bool CFavoriteMimeData::hasFormat(const QString &mimetype) const
{
    return MIME_TYPE_RABBITREMOTECONTROL_APP_FAVORITE == mimetype;
}

QStringList CFavoriteMimeData::formats() const
{
    return QStringList() << MIME_TYPE_RABBITREMOTECONTROL_APP_FAVORITE;
}
