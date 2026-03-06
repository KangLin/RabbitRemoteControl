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

/*!
 * \brief The CHistoryDatabase class
 * \ingroup DATABASE_API
 */
class CHistoryDatabase : public CDatabase
{
    Q_OBJECT
public:
    explicit CHistoryDatabase(QObject *parent = nullptr);
    ~CHistoryDatabase();

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
    [[nodiscard]] QList<HistoryItem> getAllHistory(int limit = -1, int offset = 0);
    [[nodiscard]] QList<HistoryItem> getHistoryByDate(const QDate &date);
    [[nodiscard]] QList<HistoryItem> getHistoryByDate(const QDate &start, const QDate &end, int limit = 100);
    [[nodiscard]] QList<HistoryItem> searchHistory(const QString &keyword);
    [[nodiscard]] HistoryItem getHistoryByUrl(const QString &url);
    [[nodiscard]] HistoryItem getHistoryById(int id);

    // 统计信息
    [[nodiscard]] int getHistoryCount();
    [[nodiscard]] QDateTime getLastVisitTime();

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
    [[nodiscard]] bool OnInitializeDatabase() override;
    CDatabaseUrl m_UrlDB;

    [[nodiscard]] virtual bool ExportToJson(QJsonObject &obj) override;
    [[nodiscard]] virtual bool ImportFromJson(const QJsonObject &obj) override;
    
    // CDatabase interface
protected:
    [[nodiscard]] virtual bool OnInitializeSqliteDatabase() override;
    [[nodiscard]] virtual bool OnInitializeMySqlDatabase() override;
};

void enableSqlTrace(const QString& connectionName = QSqlDatabase::defaultConnection);
