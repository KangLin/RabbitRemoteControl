// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QSerialPort>
#include "ParameterTerminalBase.h"

class CParameterSerialPort : public CParameterTerminalBase
{
    Q_OBJECT

public:
    CParameterSerialPort(CParameterOperate *parent = nullptr,
                         const QString& szPrefix = QString());

    CParameterTerminal m_Terminal;
    
    [[nodiscard]] int GetSerialPort() const;
    void SetSerialPort(int index);
    [[nodiscard]] const QString GetSerialPortName() const;
    void SetSerialPortName(const QString& szName);
    [[nodiscard]] qint32 GetBaudRate() const;
    void SetBaudRate(qint32 rate);
    [[nodiscard]] QSerialPort::Parity GetParity() const;
    void SetParity(const QSerialPort::Parity &newParity);
    [[nodiscard]] QSerialPort::DataBits GetDataBits() const;
    void SetDataBits(const QSerialPort::DataBits &newDataBits);
    [[nodiscard]] QSerialPort::StopBits GetStopBits() const;
    void SetStopBits(const QSerialPort::StopBits &newStopBits);
    [[nodiscard]] QSerialPort::FlowControl GetFlowControl() const;
    void SetFlowControl(const QSerialPort::FlowControl &newFlowControl);

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

private:
    int m_nSerialPort;
    QString m_szSerialPortName;
    qint32 m_nBaudRate;
    QSerialPort::Parity m_Parity;
    QSerialPort::DataBits m_DataBits;
    QSerialPort::StopBits m_StopBits;
    QSerialPort::FlowControl m_FlowControl;
};
