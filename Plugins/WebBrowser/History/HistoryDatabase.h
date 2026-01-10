// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QUrl>
#include <QIcon>

struct HistoryItem {
    int id;
    QIcon icon;
    QString url;
    QString title;
    QDateTime visitTime;
    int visitCount;
    QDateTime lastVisitTime;
};

class CHistoryDatabase : public QObject
{
    Q_OBJECT
public:
    explicit CHistoryDatabase(QObject *parent = nullptr);
    ~CHistoryDatabase();

    bool openDatabase(const QString &dbPath = QString());
    bool isOpen();
    void closeDatabase();

    // 历史记录操作
    bool addHistoryEntry(const QString &url, const QString &title, bool bSingle = false);
    bool updateHistoryEntry(const QString& url, const QString &title = QString(), const QIcon& icon = QIcon());
    bool updateHistoryEntry(int id, const QString &title = QString(), const QIcon& icon = QIcon());
    bool deleteHistoryEntry(int id);
    bool deleteHistoryEntry(const QString& url);
    bool deleteDomainEntries(const QString& szDomain);
    bool clearHistory(int days = 0); // 0表示清除所有
    void scheduleCleanup(int maxDays, int maxCount);

    // 查询操作
    QList<HistoryItem> getAllHistory(int limit = 100, int offset = 0);
    QList<HistoryItem> getHistoryByDate(const QDate &date);
    QList<HistoryItem> searchHistory(const QString &keyword);
    HistoryItem getHistoryByUrl(const QString &url);

    // 统计信息
    int getHistoryCount();
    QDateTime getLastVisitTime();

    bool exportToCSV(const QString &filename);
    QString escapeForCsv(const QString &text);

private:
    QSqlDatabase m_database;
    bool initializeDatabase();
};

void enableSqlTrace(const QString& connectionName = QSqlDatabase::defaultConnection);
