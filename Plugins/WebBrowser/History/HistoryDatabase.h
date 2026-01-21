// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QUrl>
#include <QIcon>
#include "Database.h"

struct HistoryItem {
    int id;
    QIcon icon;
    int iconId;
    QString url;
    QString title;
    QDateTime visitTime;
    int visitCount;
    QDateTime lastVisitTime;
};

class CHistoryDatabase : public CDatabase
{
    Q_OBJECT
public:
    explicit CHistoryDatabase(QObject *parent = nullptr);
    ~CHistoryDatabase();

    // 历史记录操作
    bool addHistoryEntry(const QString &url, const QString &title, const QIcon& icon = QIcon(), bool bSingle = false);
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
private:
    QString escapeForCsv(const QString &text);
    QString unescapeCsvField(const QString &field);
    bool validateCsvHeader(const QString &headerLine);
    QStringList parseCsvLine(const QString &line);
    bool importCsvRecord(const QStringList &fields);

private:
    QSqlDatabase m_database;
    bool OnInitializeDatabase() override;
    CDatabaseIcon m_iconDB;
};

void enableSqlTrace(const QString& connectionName = QSqlDatabase::defaultConnection);
