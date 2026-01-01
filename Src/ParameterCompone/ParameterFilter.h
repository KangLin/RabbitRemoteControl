// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QSet>
#include "Parameter.h"

class CParameterFilter : public CParameter
{
    Q_OBJECT

public:
    explicit CParameterFilter(QObject *parent = nullptr,
                              const QString& szPrefix = QString());

    virtual bool contains(const QString& szKey);

    int AddKey(const QString& szKey);
    int RemoveKey(const QString& szKey);
    int Clear();
private:
    QSet<QString> m_Key;

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
