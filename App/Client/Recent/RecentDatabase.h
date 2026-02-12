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

    int AddRecent(const RecentItem &item);
    bool DeleteRecent(int id);
    bool UpdateRecent(
        const QString &szFile, const QString& szName, const QString& szDescription);
    QList<RecentItem> GetRecents(int limit = -1, int offset = 0);

private:
    bool OnInitializeDatabase() override;
    virtual bool OnInitializeSqliteDatabase() override;
    virtual bool OnInitializeMySqlDatabase() override;
    virtual bool ExportToJson(QJsonObject &obj) override;
    virtual bool ImportFromJson(const QJsonObject &obj) override;

    CDatabaseIcon m_IconDB;
    CDatabaseFile m_FileDB;
};

