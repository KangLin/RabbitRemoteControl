// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "OperateRawStream.h"
#include "DlgSettingsRawStream.h"
#include "Plugin.h"

static Q_LOGGING_CATEGORY(log, "RawStream.Operate")

COperateRawStream::COperateRawStream(CPlugin *plugin)
    : COperateTerminal(plugin)
{
    SetParameter(&m_Parameter);
}

COperateRawStream::~COperateRawStream()
{}

QDialog *COperateRawStream::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsRawStream(&m_Parameter, parent);
}

CBackend *COperateRawStream::InstanceBackend()
{
    return nullptr;
}

int COperateRawStream::Start()
{
    if(!m_pTerminal)
        return -1;

    slotUpdateParameter(this);

    bool check = false;
    check = connect(m_pTerminal, &QTermWidget::sendData,
                    this, [&](const char* buf, int len){
                        if(!m_TcpSocket.isOpen()) return;
                        QByteArray data(buf, len);
                        //qDebug(log) << "Send data:" << data << data.toHex(':');
                        qint64 nLen = 0;
                        switch(m_Parameter.GetType()) {
                        case CParameterRawStream::TYPE::TCP:
                            nLen = m_TcpSocket.write(data);
                            break;
                        case CParameterRawStream::TYPE::NamePipe:
                            break;
                        }
                        if(GetStats())
                            GetStats()->AddSends(nLen);
                    });
    Q_ASSERT(check);
    
    check = connect(&m_TcpSocket, SIGNAL(connected()),
                    this, SIGNAL(sigRunning()));
    Q_ASSERT(check);
    check = connect(&m_TcpSocket, SIGNAL(readyRead()),
                    this, SLOT(slotReadyRead()));
    Q_ASSERT(check);
    check = connect(&m_TcpSocket, &QTcpSocket::errorOccurred,
                    this, [&](QAbstractSocket::SocketError error) {
        qCritical(log) << m_TcpSocket.errorString() << error;
        if(m_TcpSocket.isOpen())
            emit sigError(error, m_TcpSocket.errorString());
    });
    Q_ASSERT(check);
    
    switch(m_Parameter.GetType()) {
    case CParameterRawStream::TYPE::TCP: {
        auto &net = m_Parameter.m_Net;
        m_TcpSocket.connectToHost(net.GetHost(), net.GetPort());
        break;
    }
    case CParameterRawStream::TYPE::NamePipe:
        break;
    default:
        emit sigRunning();
    }

    m_pTerminal->startTerminalTeletype();

    return 0;
}

int COperateRawStream::Stop()
{
    if(m_pTerminal)
        m_pTerminal->close();

    m_TcpSocket.close();
    emit sigFinished();
    return 0;
}

void COperateRawStream::slotReadyRead()
{
    if(!m_TcpSocket.isOpen())
        return;
    auto data = m_TcpSocket.readAll();
    if(GetStats())
        GetStats()->AddReceives(data.length());
    WriteTerminal(data.data(), data.length());
}

const QString COperateRawStream::Id()
{
    QString szId;
    szId = COperate::Id();
    if(CParameterRawStream::TYPE::TCP == m_Parameter.GetType()) {
        auto &net = m_Parameter.m_Net;
        if(!net.GetHost().isEmpty())
            szId += "_" + net.GetHost()
                    + "_" + QString::number(net.GetPort());
    }
    static QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}

const QString COperateRawStream::Name()
{
    QString szName = m_Parameter.GetName();

    if(szName.isEmpty()) {
        if(CParameterRawStream::TYPE::TCP == m_Parameter.GetType()) {
            auto &net = m_Parameter.m_Net;
            if(!net.GetHost().isEmpty()) {
                if(m_Parameter.GetGlobalParameters()
                    && m_Parameter.GetGlobalParameters()->GetShowProtocolPrefix()
                    && !Protocol().isEmpty())
                    szName = Protocol() + ":";
                szName += net.GetHost()
                          + ":" + QString::number(net.GetPort());
            }
        }
    }

    if(szName.isEmpty())
        szName = COperateTerminal::Name();
    return szName;
}

const QString COperateRawStream::Description()
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

    szDescription += tr("Raw stream type: ") + CParameterRawStream::GetTypeName(m_Parameter.GetType()) + "\n";

    if(CParameterRawStream::TYPE::TCP == m_Parameter.GetType()) {
        auto &net = m_Parameter.m_Net;
        if(!net.GetHost().isEmpty())
            szDescription += tr("Server address: ") + net.GetHost()
                             + ":" + QString::number(net.GetPort()) + "\n";
    }

    if(GetSecurityLevel() != SecurityLevel::No)
        szDescription += tr("Security level: ") + GetSecurityLevelString() + "\n";

    if(!GetPlugin()->Description().isEmpty())
        szDescription += tr("Description: ") + GetPlugin()->Description();

    return szDescription;
}
