// Author: Kang Lin <kl222@126.com>

#ifndef CParameterSSH_H
#define CParameterSSH_H

#pragma once

#include "ParameterNet.h"

class PLUGIN_EXPORT CParameterSSHTunnel : public CParameterOperate
{
    Q_OBJECT

public:
    explicit CParameterSSHTunnel(CParameterOperate *parent = nullptr,
                           const QString& szPrefix = QString());

    CParameterNet m_Net;

    QString GetSourceHost() const;
    int SetSourceHost(const QString szHost);

    quint16 GetSourcePort() const;
    int SetSourcePort(const quint16 nPort);

    QString GetPcapFile() const;
    int SetPcapFile(const QString& szFile);

private:
    QString m_szSourceHost;
    quint16 m_nSourcePort;

    QString m_pcapFile;
    
    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};

#endif // CParameterSSH_H
