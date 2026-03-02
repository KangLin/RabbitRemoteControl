// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Database.h"

class PLUGIN_EXPORT CDatabaseFilter : public CDatabase
{
public:
    explicit CDatabaseFilter(const QString& szSuffix = QString(), QObject* parent = nullptr);

    bool contains(const QString& szKey);
    int AddKey(const QString& szKey);
    int RemoveKey(const QString& szKey);
    int Clear();
    virtual bool isEmpty();

    virtual bool OnInitializeDatabase() override;
    virtual bool ExportToJson(QJsonObject &obj) override;
    virtual bool ImportFromJson(const QJsonObject &obj) override;

private:
    QString m_szTableName;

protected:
    virtual bool OnInitializeSqliteDatabase() override;
    virtual bool OnInitializeMySqlDatabase() override;
};

