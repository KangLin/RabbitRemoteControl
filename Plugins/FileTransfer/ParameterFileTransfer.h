// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterSSH.h"

class CParameterFileTransfer : public CParameterOperate
{
    Q_OBJECT

public:
    explicit CParameterFileTransfer(QObject *parent = nullptr,
                                    const QString& szPrefix = QString());
    
    CParameterNet m_Net;
    CParameterSSH m_SSH;

public:
    enum class Protocol{
        FTP,
        SFTP
    };
    Q_ENUM(Protocol)
    Protocol GetProtocol() const;
    void SetProtocol(Protocol newType);
    
private:
    Protocol m_Protocol;
    
    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
