// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Database.h"

class PLUGIN_EXPORT CDatabaseFilter : public CDatabase
{
public:
    explicit CDatabaseFilter(const QString& szSuffix = QString(), QObject* parent = nullptr);

    [[nodiscard]] bool contains(const QString& szKey);
    int AddKey(const QString& szKey);
    int RemoveKey(const QString& szKey);
    int Clear();
    [[nodiscard]] virtual bool isEmpty();

    [[nodiscard]] virtual bool OnInitializeDatabase() override;
    [[nodiscard]] virtual bool ExportToJson(QJsonObject &obj) override;
    [[nodiscard]] virtual bool ImportFromJson(const QJsonObject &obj) override;

private:
    QString m_szTableName;

protected:
    [[nodiscard]] virtual bool OnInitializeSqliteDatabase() override;
    [[nodiscard]] virtual bool OnInitializeMySqlDatabase() override;
};

