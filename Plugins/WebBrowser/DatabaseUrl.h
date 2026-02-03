// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QIcon>
#include "Database.h"

class CDatabaseUrl : public CDatabase
{
    Q_OBJECT
public:
    explicit CDatabaseUrl(QObject *parent = nullptr);

    struct UrlItem {
        int id;
        QString szUrl;
        QString szTitle;
        QIcon icon;
        QDateTime visit_time;

        UrlItem()
            : id(0)
            , iconId(0)
        {}
        int iconId;
    };

    int AddUrl(const QString& url, const QString& title = QString(), const QIcon& icon = QIcon());
    bool DeleteUrl(const QString& url);
    bool DeleteUrl(int id);
    bool UpdateUrl(const QString& url, const QString& title = QString(), const QIcon& icon = QIcon());
    bool UpdateUrl(int id, const QString& title = QString(), const QIcon& icon = QIcon());
    UrlItem GetItem(int id);
    UrlItem GetItem(const QString& url);
    int GetId(const QString& url);
    QList<int> GetDomain(const QString& szDomain);
    QList<UrlItem> Search(const QString& keyword);

    virtual bool OnInitializeDatabase() override;
    virtual bool ExportToJson(QJsonObject& obj) override;
    virtual bool ImportFromJson(const QJsonObject& obj) override;

private:
    CDatabaseIcon m_iconDB;
};

