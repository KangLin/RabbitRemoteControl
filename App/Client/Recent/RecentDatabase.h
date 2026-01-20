// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QIcon>
#include "Database.h"

class CRecentDatabase : public CDatabase
{
    Q_OBJECT

public:
    explicit CRecentDatabase(QObject *parent = nullptr);
    ~CRecentDatabase();

    struct RecentItem {
        int id;
        QString szOperateId;
        QIcon icon;
        QString szName;
        QString szProtocol;
        QString szType;
        QString szFile;
        QDateTime time;
        QString szDescription;
        RecentItem() : id(0) {
        }
    };

    int addRecent(const RecentItem &item);
    bool deleteRecent(int id);
    bool updateRecent(
        const QString &szFile, const QString& szName, const QString& szDescription);
    QList<RecentItem> getRecents(int limit = -1, int offset = 0);

private:
    bool onInitializeDatabase() override;
    CDatabaseIcon m_IconDB;
};

