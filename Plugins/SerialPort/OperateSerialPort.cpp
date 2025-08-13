#include <QSerialPortInfo>
#include <QLoggingCategory>
#include "OperateSerialPort.h"
#include "DlgSettingsSerialPort.h"

static Q_LOGGING_CATEGORY(log, "SerialPort.Operate")

COperateSerialPort::COperateSerialPort(CPlugin *plugin)
    : COperateTerminal(plugin)
{
    SetParameter(&m_Parameter);
}

COperateSerialPort::~COperateSerialPort()
{}

QDialog *COperateSerialPort::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsSerialPort(&m_Parameter, parent);
}

CBackend *COperateSerialPort::InstanceBackend()
{
    return nullptr;
}

int COperateSerialPort::Start()
{
    if(!m_pTerminal)
        return -1;

    slotUpdateParameter(this);

    bool check = false;
    check = connect(m_pTerminal, &QTermWidget::sendData,
                    this, [&](const char* buf, int len){
                        if(!m_SerialPort.isOpen()) return;
                        QByteArray data(buf, len);
                        //qDebug(log) << "Send data:" << data << data.toHex(':');
                        m_SerialPort.write(data);
                    });
    Q_ASSERT(check);
    check = connect(&m_SerialPort, SIGNAL(readyRead()),
                    this, SLOT(slotReadyRead()));
    Q_ASSERT(check);
    check = connect(&m_SerialPort, &QSerialPort::errorOccurred,
                    this, [&](QSerialPort::SerialPortError error) {
        qCritical(log) << m_SerialPort.errorString() << error;
        emit sigError(error, m_SerialPort.errorString());
    });
    Q_ASSERT(check);

    if(-1 == m_Parameter.GetSerialPort()
        && m_Parameter.GetSerialPortName().isEmpty()) {
        qCritical(log) << "Serial port is invalid";
        return -1;
    }
    
    int index = m_Parameter.GetSerialPort();
    if(0 < index && index < QSerialPortInfo::availablePorts().size()) {
        QSerialPortInfo info = QSerialPortInfo::availablePorts().at(index);
        if(info.portName() == m_Parameter.GetSerialPortName())
            m_SerialPort.setPort(info);
        else
            m_SerialPort.setPortName(m_Parameter.GetSerialPortName());
    } else {
        if(m_Parameter.GetSerialPortName().isEmpty()) {
            qCritical(log) << "Serial port is invalid";
            return -1;
        }
        else
            m_SerialPort.setPortName(m_Parameter.GetSerialPortName());
    }
    m_SerialPort.setBaudRate(m_Parameter.GetBaudRate());
    m_SerialPort.setParity(m_Parameter.GetParity());
    m_SerialPort.setDataBits(m_Parameter.GetDataBits());
    m_SerialPort.setStopBits(m_Parameter.GetStopBits());
    m_SerialPort.setFlowControl(m_Parameter.GetFlowControl());
    if(!m_SerialPort.open(QIODevice::ReadWrite)) {
        QString szError;
        szError = tr("Open Serial port [%1] fail errno[%2]: %3").
                  arg(m_Parameter.GetSerialPortName(),
                      QString::number(m_SerialPort.error()), m_SerialPort.errorString());
        qCritical(log) << szError;
        emit sigError(m_SerialPort.error(), szError);
        return -1;
    }

    m_pTerminal->startTerminalTeletype();
    emit sigRunning();
    return 0;
}

int COperateSerialPort::Stop()
{
    if(m_pTerminal)
        m_pTerminal->close();

    m_SerialPort.close();
    emit sigFinished();
    return 0;
}

void COperateSerialPort::slotReadyRead()
{
    if(!m_SerialPort.isOpen())
        return;
    auto data = m_SerialPort.readAll();
    WriteTerminal(data.data(), data.length());
}
