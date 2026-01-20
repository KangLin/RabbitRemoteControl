// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QMimeData>
#include <QModelIndexList>

class CFavoriteMimeData : public QMimeData
{
    Q_OBJECT

public:
    CFavoriteMimeData(QObject *parent = nullptr);
    QModelIndexList m_Items;
};
