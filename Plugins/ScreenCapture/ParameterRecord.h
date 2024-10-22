// Author: Kang Lin <kl222@126.com>

#ifndef PARAMETERRECORD_H
#define PARAMETERRECORD_H

#pragma once

#include "Parameter.h"
class CParameterRecord : public CParameter
{
    Q_OBJECT
public:
    explicit CParameterRecord(QObject *parent = nullptr,
                              const QString& szPrefix = QString());

    const bool GetEnable() const;
    int SetEnable(bool bEnable);

    const QString GetUrl() const;
    int SetUrl(const QString& szFile);

    const QString GetPath() const;
    int SetPath(const QString& szPath);

private:
    bool m_bEnable;
    QString m_szUrl;
    QString m_szPath;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};

#endif // PARAMETERRECORD_H
