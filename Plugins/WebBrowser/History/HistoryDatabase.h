// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QDateTime>
#include <QUrl>
#include <QIcon>
#include "DatabaseUrl.h"

struct HistoryItem {
    int id;
    QIcon icon;
    QString url;
    QString title;
    QDateTime visitTime;
};

class CHistoryDatabase : public CDatabase
{
    Q_OBJECT
public:
    static CHistoryDatabase* Instance(CParameterDatabase* para = nullptr);

    // 历史记录操作
    bool addHistoryEntry(const QString &url);
    bool addHistoryEntry(const QString &url, const QString& title, const QDateTime& time);
    bool updateHistoryEntry(const QString& url, const QString &title = QString(), const QIcon& icon = QIcon());
    bool updateHistoryEntry(int id, const QString &title = QString(), const QIcon& icon = QIcon());
    bool deleteHistoryEntry(int id);
    bool deleteHistoryEntry(const QString& url);
    bool deleteDomainEntries(const QString& szDomain);
    bool clearHistory(int days = 0); // 0表示清除所有
    void scheduleCleanup(int maxDays, int maxCount);

    // 查询操作
    QList<HistoryItem> getAllHistory(int limit = -1, int offset = 0);
    QList<HistoryItem> getHistoryByDate(const QDate &date);
    QList<HistoryItem> getHistoryByDate(const QDate &start, const QDate &end, int limit = 100);
    QList<HistoryItem> searchHistory(const QString &keyword);
    HistoryItem getHistoryByUrl(const QString &url);
    HistoryItem getHistoryById(int id);

    // 统计信息
    int getHistoryCount();
    QDateTime getLastVisitTime();

    bool importFromCSV(const QString& filename);
    bool exportToCSV(const QString &filename);
    bool importFromJson(const QString& filename);
    bool exportToJson(const QString& filename);
private:
    QString escapeForCsv(const QString &text);
    QString unescapeCsvField(const QString &field);
    bool validateCsvHeader(const QString &headerLine);
    QStringList parseCsvLine(const QString &line);
    bool importCsvRecord(const QStringList &fields);

private:
    explicit CHistoryDatabase(QObject *parent = nullptr);
    ~CHistoryDatabase();
    bool OnInitializeDatabase() override;
    CDatabaseUrl m_UrlDB;

    virtual bool ExportToJson(QJsonObject &obj) override;
    virtual bool ImportFromJson(const QJsonObject &obj) override;
    
    // CDatabase interface
protected:
    virtual bool OnInitializeSqliteDatabase() override;
    virtual bool OnInitializeMySqlDatabase() override;
};

void enableSqlTrace(const QString& connectionName = QSqlDatabase::defaultConnection);
