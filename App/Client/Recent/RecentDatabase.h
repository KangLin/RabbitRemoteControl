// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include "Database.h"
#include "RabbitCommonDir.h"

class CRecentDatabase : public CDatabase
{
    Q_OBJECT

public:
    explicit CRecentDatabase(QObject *parent = nullptr);
    virtual ~CRecentDatabase();

    struct RecentItem {
        int id;
        QString szOperateId;
        QIcon icon;
        QString szName;
        QString szProtocol;
        QString szType;
        int SetFile(const QString& file);
        QString GetFile();
        QDateTime time;
        QString szDescription;
        RecentItem();
    private:
        QString szFile;
        friend CRecentDatabase;
    };

    int AddRecent(const RecentItem &item);
    bool DeleteRecent(int id);
    bool UpdateRecent(const RecentItem &item);
    QList<RecentItem> GetRecents(int limit = -1, int offset = 0);

    bool HasFileContents(const RecentItem &item);

private:
    bool OnInitializeDatabase() override;
    virtual bool OnInitializeSqliteDatabase() override;
    virtual bool OnInitializeMySqlDatabase() override;
    virtual bool ExportToJson(QJsonObject &obj) override;
    virtual bool ImportFromJson(const QJsonObject &obj) override;

    bool UpdateRecent(const QString &szFile, const QString& szName, const QString& szDescription);

    CDatabaseIcon m_IconDB;
    CDatabaseFile m_FileDB;
};

