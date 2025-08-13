#ifndef TERMINALPARAMETER_H
#define TERMINALPARAMETER_H

#include "ParameterTerminalBase.h"

class CTerminalParameter : public CParameterTerminalBase
{
    Q_OBJECT
public:
    explicit CTerminalParameter(CParameterOperate *parent = nullptr,
                                const QString& szPrefix = QString());
    
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
    [[nodiscard]] const QString GetLasterDirectory() const;
    int SetLasterDirectory(const QString& d);
private:
    QString m_szLasterDirectory;
    
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
    
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};

#endif // TERMINALPARAMETER_H
