// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateTerminal.h"
#include "QTelnet.h"
#include "ParameterTelnet.h"

class COperateTelnet : public COperateTerminal
{
    Q_OBJECT
public:
    explicit COperateTelnet(CPlugin *parent);
    virtual ~COperateTelnet();

public:
    [[nodiscard]] virtual CBackend *InstanceBackend() override;
public slots:
    virtual int Start() override;
    virtual int Stop() override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private Q_SLOTS:
    void slotStateChanged(QAbstractSocket::SocketState state);
    void slotNewData(const char* buf, int len);

private:
    CParameterTelnet m_Parameters;
    QTelnet m_Telnet;
    bool m_bLogin;
    
    // COperate interface
public:
    virtual const QString Id() override;
    virtual const QString Name() override;
    virtual const QString Description() override;
    
};
