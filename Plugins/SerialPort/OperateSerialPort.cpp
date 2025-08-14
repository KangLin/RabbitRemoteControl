#include <QSerialPortInfo>
#include <QLoggingCategory>
#include "OperateSerialPort.h"
#include "DlgSettingsSerialPort.h"
#include "Plugin.h"

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
                        qint64 nLen = m_SerialPort.write(data);
                        if(GetStats())
                            GetStats()->AddSends(nLen);
                    });
    Q_ASSERT(check);
    check = connect(&m_SerialPort, SIGNAL(readyRead()),
                    this, SLOT(slotReadyRead()));
    Q_ASSERT(check);
    check = connect(&m_SerialPort, &QSerialPort::errorOccurred,
                    this, [&](QSerialPort::SerialPortError error) {
        qCritical(log) << m_SerialPort.errorString() << error;
        if(m_SerialPort.isOpen())
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
    if(GetStats())
        GetStats()->AddReceives(data.length());
    WriteTerminal(data.data(), data.length());
}

const QString COperateSerialPort::Id()
{
    QString szId;
    szId = COperateTerminal::Id();
    if(!m_Parameter.GetSerialPortName().isEmpty())
        szId += "_" + m_Parameter.GetSerialPortName();
    static QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}

const QString COperateSerialPort::Name()
{
    QString szName;

    if(!m_Parameter.GetName().isEmpty())
        return m_Parameter.GetName();

    if(!m_Parameter.GetSerialPortName().isEmpty())
        szName = m_Parameter.GetSerialPortName();

    if(szName.isEmpty())
        szName = COperateTerminal::Name();
    return szName;
}

const QString COperateSerialPort::Description()
{
    QString szDescription;
    if(!Name().isEmpty())
        szDescription = tr("Name: ") + Name() + "\n";

    if(!GetTypeName().isEmpty())
        szDescription += tr("Type: ") + GetTypeName() + "\n";

    if(!Protocol().isEmpty()) {
        szDescription += tr("Protocol: ") + Protocol();
#ifdef DEBUG
        if(!GetPlugin()->DisplayName().isEmpty())
            szDescription += " - " + GetPlugin()->DisplayName();
#endif
        szDescription += "\n";
    }

    if(!m_Parameter.GetSerialPortName().isEmpty())
        szDescription += tr("Serial port: ") + m_Parameter.GetSerialPortName() + "\n";

    if(GetSecurityLevel() != SecurityLevel::No)
        szDescription += tr("Security level: ") + GetSecurityLevelString() + "\n";

    if(!GetPlugin()->Description().isEmpty())
        szDescription += tr("Description: ") + GetPlugin()->Description();

    return szDescription;
}
