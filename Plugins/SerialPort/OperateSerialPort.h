// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateTerminal.h"
#include "ParameterSerialPort.h"

class COperateSerialPort : public COperateTerminal
{
    Q_OBJECT
public:
    explicit COperateSerialPort(CPlugin *plugin);
    virtual ~COperateSerialPort();

    [[nodiscard]] const QString Name() override;
    [[nodiscard]] const QString Description() override;

    virtual int Start() override;
    virtual int Stop() override;

    // COperate interface
private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

    // COperateTerminal interface
public:
    virtual CBackend *InstanceBackend() override;

private Q_SLOTS:
    void slotReadyRead();

private:
    CParameterSerialPort m_Parameter;
    QSerialPort m_SerialPort;
};

