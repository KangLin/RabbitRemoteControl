// Author: Kang Lin <kl222@126.com>

#pragma once
#include "plugin_export.h"
#include "ParameterOperate.h"
#include "ParameterTerminal.h"
#include "ParameterSSH.h"

class PLUGIN_EXPORT CParameterTerminalBase : public CParameterOperate
{
    Q_OBJECT
public:
    explicit CParameterTerminalBase(QObject *parent = nullptr);

    CParameterTerminal m_Terminal;
    CParameterSSH m_SSH;

public:
    int SetShell(const QString& shell);
    QString GetShell();
private:
    QString m_szShell;

public:
    int SetShellParameters(const QString& para);
    QString GetShellParameters();
private:
    QString m_szShellParameters;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

    // CParameterOperate interface
protected slots:
    virtual void slotSetGlobalParameters() override;
};
