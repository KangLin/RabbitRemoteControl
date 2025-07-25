// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateTerminal.h"
#include "ParameterTerminalSSH.h"

class COperateSSH : public COperateTerminal
{
    Q_OBJECT

public:
    explicit COperateSSH(CPlugin *parent);
    virtual ~COperateSSH();

    [[nodiscard]] virtual CBackend *InstanceBackend() override;

    [[nodiscard]] virtual const QString Id() override;
    [[nodiscard]] virtual const QString Name() override;
    [[nodiscard]] virtual const QString Description() override;
    [[nodiscard]] virtual SecurityLevel GetSecurityLevel() override;

Q_SIGNALS:
    void sigReceiveData(const QByteArray& data);

private:
    CParameterTerminalSSH m_Parameters;
    
protected:
    virtual int Initial() override;
    virtual int Clean() override;

private:
    [[nodiscard]] virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private Q_SLOTS:
    void slotReceiveData(const QByteArray& data);
};
