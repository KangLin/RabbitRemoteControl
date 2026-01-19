// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QIcon>

class CRecentDatabase : public QObject
{
    Q_OBJECT

public:
    explicit CRecentDatabase(QObject *parent = nullptr);
    ~CRecentDatabase();

    struct Item {
        int id;
        QString szOperateId;
        QIcon icon;
        QString szName;
        QString szProtocol;
        QString szType;
        QString szFile;
        QDateTime time;
        QString szDescription;
        Item() : id(0) {
        }
    };

    bool openDatabase(const QString &dbPath = QString());
    bool isOpen();
    void closeDatabase();

    int addRecent(const Item &item);
    bool deleteRecent(int id);
    bool updateRecent(
        const QString &szFile, const QString& szName, const QString& szDescription);
    QList<Item> getRecents(int limit = -1, int offset = 0);

    int getIcon(const QIcon& icon);
    QIcon getIcon(int id);

Q_SIGNALS:
    void sigChanged();

private:
    QSqlDatabase m_database;
    bool initializeDatabase();
};

