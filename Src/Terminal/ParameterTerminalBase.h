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
    explicit CParameterTerminalBase(CParameterOperate *parent = nullptr,
                                    const QString& szPrefix = QString());

    CParameterTerminal m_Terminal;

public:
    [[nodiscard]] const QString GetShellName() const;
    int SetShellName(const QString& name);
private:
    QString m_szShellName;

public:
    int SetShell(const QString& shell);
    [[nodiscard]] const QString GetShell() const;
private:
    QString m_szShell;

public:
    int SetShellParameters(const QString& para);
    [[nodiscard]] const QString GetShellParameters() const;
private:
    QString m_szShellParameters;

public:
    [[nodiscard]] const QString GetCurrentDirectory() const;
    int SetCurrentDirectory(const QString& d);
private:
    QString m_szCurrentDirectory;

public:
    [[nodiscard]] bool GetRestoreDirectory() const;
    int SetRestoreDirectory(bool bEnable);
private:
    bool m_bRestoreDirectory;

public:
    [[nodiscard]] bool GetEnableTitleChanged() const;
    void SetEnableTitleChanged(bool newTitleChanged);
private:
Q_SIGNALS:
    void sigEnableTitleChanged(bool titleChanged);
private:
    bool m_bTitleChanged;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

    // CParameterOperate interface
protected slots:
    virtual void slotSetGlobalParameters() override;
};
