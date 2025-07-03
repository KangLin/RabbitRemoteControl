// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterOperate.h"
#include "ParameterTerminalBase.h"
#include "ParameterNet.h"

class CParameterTelnet : public CParameterTerminalBase
{
    Q_OBJECT
public:
    explicit CParameterTelnet(
        CParameterOperate *parent = nullptr,
        const QString& szPrefix = QString());
    virtual ~CParameterTelnet();

    CParameterNet m_Net;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

public:
    //! Login Regular Expression
    QString GetLogin() const;
    void SetLogin(const QString &newLogin);
    //! Password Regular Expression
    QString GetPassword() const;
    void SetPassword(const QString &newPassword);

private:
    QString m_szLogin;    //! Login Regular Expression
    QString m_szPassword; //! Password Regular Expression
};
