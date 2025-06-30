// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateTerminal.h"
#include "ParameterTerminalBase.h"

class COperateSSH : public COperateTerminal
{
    Q_OBJECT

public:
    explicit COperateSSH(CPlugin *parent);
    virtual ~COperateSSH();

    virtual CBackend *InstanceBackend() override;

Q_SIGNALS:
    void sigReceiveData(const QByteArray& data);

private:
    CParameterTerminalBase m_Parameters;
    
    // COperate interface
protected:
    virtual int Initial() override;
    virtual int Clean() override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private Q_SLOTS:
    void slotReceiveData(const QByteArray& data);
};
