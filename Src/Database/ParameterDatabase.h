// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterNet.h"

class PLUGIN_EXPORT CParameterDatabase : public CParameterOperate
{
    Q_OBJECT
public:
    explicit CParameterDatabase(QObject *parent = nullptr,
                                const QString& szPrefix = QString());
    virtual CParameterDatabase& operator=(const CParameterDatabase &in);
    CParameterNet m_Net;

    QString GetType() const;
    void SetType(const QString &newType);
    QString GetDatabaseName() const;
    void SetDatabaseName(const QString &newDatabaseName);
    QString GetOptions() const;
    void SetOptions(const QString &newOptions);

    static QSet<QString> GetSupportDatabase();
    const QString Details() const;

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

private:
    QString m_szType;
    QString m_szDatabaseName;
    QString m_szOptions;
};
