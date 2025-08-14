// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QAtomicInteger>
#include <QAtomicInt>
#include "ParameterOperate.h"

class PLUGIN_EXPORT CStats : public CParameterOperate
{
    Q_OBJECT

public:
    explicit CStats(CParameterOperate* parent = nullptr,
                    const QString& szPrefix = QString());

    [[nodiscard]] static QString Convertbytes(quint64 bytes);

    void AddSends(quint64 size);
    void AddReceives(quint64 size);

private:
    QAtomicInteger<quint64> m_TotalSends;
    QAtomicInteger<quint64> m_TotalReceives;
    
    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
