// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ParameterSerialPort.h"
#include "ParameterPlugin.h"

static Q_LOGGING_CATEGORY(log, "SerialPort.Parameter")

CParameterSerialPort::CParameterSerialPort(
    CParameterOperate *parent, const QString &szPrefix)
    : CParameterTerminalBase{parent, szPrefix}
    , m_nSerialPort(-1)
    , m_nBaudRate(9600)
    , m_DataBits(QSerialPort::DataBits::Data8)
    , m_Parity(QSerialPort::NoParity)
    , m_StopBits(QSerialPort::OneStop)
    , m_FlowControl(QSerialPort::HardwareControl)
{}

int CParameterSerialPort::OnLoad(QSettings &set)
{
    set.beginGroup("SerialPort");
    SetSerialPort(set.value("Index", GetSerialPort()).toInt());
    SetSerialPortName(set.value("Name", GetSerialPortName()).toString());
    SetBaudRate(set.value("BaudRate", GetBaudRate()).toInt());
    SetDataBits((QSerialPort::DataBits)set.value("DataBits", (int)GetDataBits()).toInt());
    SetParity((QSerialPort::Parity)set.value("Parity", (int)GetParity()).toInt());
    SetStopBits((QSerialPort::StopBits)set.value("StopBits", (int)GetStopBits()).toInt());
    SetFlowControl((QSerialPort::FlowControl)set.value("FlowControl", (int)GetFlowControl()).toInt());
    set.endGroup();
    return CParameterTerminalBase::OnLoad(set);
}

int CParameterSerialPort::OnSave(QSettings &set)
{
    set.beginGroup("SerialPort");
    set.setValue("Index", GetSerialPort());
    set.setValue("Name", GetSerialPortName());
    set.setValue("BaudRate", GetBaudRate());
    set.setValue("DataBits", (int)GetDataBits());
    set.setValue("Parity", (int)GetParity());
    set.setValue("StopBits", (int)GetStopBits());
    set.setValue("FlowControl", (int)GetFlowControl());
    set.endGroup();
    return CParameterTerminalBase::OnSave(set);
}

int CParameterSerialPort::GetSerialPort() const
{
    return m_nSerialPort;
}

void CParameterSerialPort::SetSerialPort(int index)
{
    if(m_nSerialPort == index)
        return;
    m_nSerialPort = index;
    SetModified(true);
}

const QString CParameterSerialPort::GetSerialPortName() const
{
    return m_szSerialPortName;
}

void CParameterSerialPort::SetSerialPortName(const QString &szName)
{
    if(m_szSerialPortName == szName)
        return;
    m_szSerialPortName = szName;
    SetModified(true);
}

qint32 CParameterSerialPort::GetBaudRate() const
{
    return m_nBaudRate;
}

void CParameterSerialPort::SetBaudRate(qint32 rate)
{
    if(m_nBaudRate == rate)
        return;
    m_nBaudRate = rate;
    SetModified(true);
}

QSerialPort::Parity CParameterSerialPort::GetParity() const
{
    return m_Parity;
}

void CParameterSerialPort::SetParity(const QSerialPort::Parity &newParity)
{
    if(m_Parity == newParity)
        return;
    m_Parity = newParity;
    SetModified(true);
}

QSerialPort::DataBits CParameterSerialPort::GetDataBits() const
{
    return m_DataBits;
}

void CParameterSerialPort::SetDataBits(const QSerialPort::DataBits &newDataBits)
{
    if(m_DataBits == newDataBits)
        return;
    m_DataBits = newDataBits;
    SetModified(true);
}

QSerialPort::StopBits CParameterSerialPort::GetStopBits() const
{
    return m_StopBits;
}

void CParameterSerialPort::SetStopBits(const QSerialPort::StopBits &newStopBits)
{
    if(m_StopBits == newStopBits)
        return;
    m_StopBits = newStopBits;
    SetModified(true);
}

QSerialPort::FlowControl CParameterSerialPort::GetFlowControl() const
{
    return m_FlowControl;
}

void CParameterSerialPort::SetFlowControl(const QSerialPort::FlowControl &newFlowControl)
{
    if(m_FlowControl == newFlowControl)
        return;
    m_FlowControl = newFlowControl;
    SetModified(true);
}
