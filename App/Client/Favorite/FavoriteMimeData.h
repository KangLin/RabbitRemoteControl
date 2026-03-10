// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QMimeData>
#include <QModelIndexList>

#define MIME_TYPE_RABBITREMOTECONTROL_APP_FAVORITE "rabbit/remotecontrol/App/Favorite"
class CFavoriteMimeData : public QMimeData
{
    Q_OBJECT

public:
    CFavoriteMimeData(QObject *parent = nullptr);
    QModelIndexList m_Items;

    virtual bool hasFormat(const QString &mimetype) const override;
    virtual QStringList formats() const override;
};
