// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QIcon>
#include "Database.h"

class CDatabaseUrl : public CDatabase
{
    Q_OBJECT
public:
    explicit CDatabaseUrl(const QString &szSuffix = QString(),
                          QObject *parent = nullptr);

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
    [[nodiscard]] UrlItem GetItem(int id);
    [[nodiscard]] UrlItem GetItem(const QString& url);
    [[nodiscard]] int GetId(const QString& url);
    [[nodiscard]] QList<int> GetDomain(const QString& szDomain);
    [[nodiscard]] QList<UrlItem> Search(const QString& keyword);

    [[nodiscard]] virtual bool ExportToJson(QJsonObject& obj) override;
    [[nodiscard]] virtual bool ImportFromJson(const QJsonObject& obj) override;

private:
    QString m_szTableName;
    CDatabaseIcon m_iconDB;
    
    // CDatabase interface
protected:
    [[nodiscard]] virtual bool OnInitializeDatabase() override;
    [[nodiscard]] virtual bool OnInitializeSqliteDatabase() override;
    [[nodiscard]] virtual bool OnInitializeMySqlDatabase() override;
};

