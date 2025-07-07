// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QAbstractSocket>
#include <QRegularExpression>

#include "OperateTelnet.h"
#include "DlgSettingsTelnet.h"
#include "Plugin.h"

static Q_LOGGING_CATEGORY(log, "Plugin.Telnet")

COperateTelnet::COperateTelnet(CPlugin *parent)
    : COperateTerminal(parent)
    , m_Telnet(QTelnet::TCP)
    , m_bLogin(false)
{
    qDebug(log) << Q_FUNC_INFO;
    auto &net = m_Parameters.m_Net;
    net.SetPort(23);
    QList<CParameterUser::TYPE> lstType;
    lstType << CParameterUser::TYPE::UserPassword;
    net.m_User.SetType(lstType);
    SetParameter(&m_Parameters);
}

COperateTelnet::~COperateTelnet()
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateTelnet::SecurityLevel COperateTelnet::GetSecurityLevel()
{
    return COperateTelnet::SecurityLevel::Risky;
}

QDialog *COperateTelnet::OnOpenDialogSettings(QWidget *parent)
{
    return new CDlgSettingsTelnet(&m_Parameters, parent);
}

CBackend *COperateTelnet::InstanceBackend()
{
    return nullptr;
}

int COperateTelnet::Start()
{
    if(!m_pTerminal)
        return -1;

    slotUpdateParameter(this);
    bool check = false;
    check = connect(&m_Telnet, &QTelnet::stateChanged,
                    this, [=](QAbstractSocket::SocketState socketState){
                        if(socketState == QAbstractSocket::ConnectedState) {
                            emit sigRunning();
                        } else if(socketState == QAbstractSocket::UnconnectedState) {
                            emit sigStop();
                        }
                    });
    Q_ASSERT(check);
    check = connect(&m_Telnet, &QTelnet::error,
                    this, [=](QAbstractSocket::SocketError err){
                        QString szErr = tr("Telnet error:\n%1.").arg(m_Telnet.errorString());
                        emit sigError(err, szErr);
                        emit sigStop();
                    });
    Q_ASSERT(check);
    check = connect(&m_Telnet, SIGNAL(newData(const char*, int)),
                    this, SLOT(slotNewData(const char*, int)));
    Q_ASSERT(check);
    check = connect(m_pTerminal, &QTermWidget::sendData,
                    this, [&](const char* buf, int len){
        QByteArray data(buf, len);
        //qDebug(log) << "Send data:" << data << data.toHex(':');
        m_Telnet.sendData(data);
    });
    Q_ASSERT(check);
    check = connect(&m_Telnet, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                    this, SLOT(slotStateChanged(QAbstractSocket::SocketState)));
    Q_ASSERT(check);

    auto &net = m_Parameters.m_Net;
    m_Telnet.setCustomCR();
    m_Telnet.connectToHost(net.GetHost(), net.GetPort());

    m_pTerminal->startTerminalTeletype();

    emit sigRunning();
    return 0;
}

int COperateTelnet::Stop()
{
    if(m_pTerminal)
        m_pTerminal->close();

    m_Telnet.disconnectFromHost();
    emit sigFinished();
    return 0;
}

void COperateTelnet::slotStateChanged(QAbstractSocket::SocketState state)
{
    switch( state )
    {
    case QAbstractSocket::UnconnectedState:
        qDebug(log) << tr("Unconnected");
        break;
    case QAbstractSocket::HostLookupState:
        qDebug(log) << tr("Resolve DNS %1").arg(m_Telnet.peerName());
        break;
    case QAbstractSocket::ConnectingState:
        qDebug(log) << tr("Connecting a %1").arg(m_Telnet.peerInfo());
        break;
    case QAbstractSocket::ConnectedState:
        qDebug(log) << tr("Connected a %1").arg(m_Telnet.peerInfo());
        break;
    case QAbstractSocket::BoundState:
        qDebug(log) << tr("Bound");
        break;
    case QAbstractSocket::ListeningState:
        qDebug(log) << tr("Listening");
        break;
    case QAbstractSocket::ClosingState:
        qDebug(log) << tr("Close");
        break;
    }
}

const QString COperateTelnet::Id()
{
    QString szId = COperateTerminal::Id();
    
    auto &net = m_Parameters.m_Net;
    if(!net.GetHost().isEmpty())
        szId += "_" + net.GetHost()
                + "_" + QString::number(net.GetPort());
    
    static QRegularExpression exp("[-@:/#%!^&* \\.]");
    szId = szId.replace(exp, "_");
    return szId;
}

const QString COperateTelnet::Name()
{
    QString szName;

    auto &net = m_Parameters.m_Net;
    if(!net.GetHost().isEmpty()) {
        if(m_Parameters.GetGlobalParameters()
            && m_Parameters.GetGlobalParameters()->GetShowProtocolPrefix())
            szName = Protocol() + ":";
        szName += net.GetHost()
                  + ":" + QString::number(net.GetPort());
    }

    if(szName.isEmpty())
        szName = COperateTerminal::Name();
    return szName;
}

const QString COperateTelnet::Description()
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

    auto &net = m_Parameters.m_Net;
    if(!net.GetHost().isEmpty())
        szDescription += tr("Server address: ") + net.GetHost()
                         + ":" + QString::number(net.GetPort()) + "\n";
    else {
        szDescription += tr("Shell path: ") + m_Parameters.GetShell() + "\n";
        if(!m_Parameters.GetShellParameters().isEmpty())
            szDescription += tr("Shell parameters: ") + m_Parameters.GetShellParameters() + "\n";
    }

    if(!GetPlugin()->Description().isEmpty())
        szDescription += tr("Description: ") + GetPlugin()->Description();
    
    return szDescription;
}

void COperateTelnet::slotNewData(const char *buf, int len)
{
    //QByteArray data(buf, len);
    //qDebug(log) << "Receive data:" << data << data.toHex(':');
    WriteTerminal(buf, len);

    if(!m_bLogin) {
        QRegularExpression reLogin(m_Parameters.GetLogin(), QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = reLogin.match(QByteArray(buf, len));
        bool bLogin = match.hasMatch();
        if(bLogin) {
            QString user = m_Parameters.m_Net.m_User.GetUser();
            if(!user.isEmpty()) {
                m_Telnet.sendData(user.toStdString().c_str(), user.length());
                m_Telnet.sendData("\n", 1);
            }
            qDebug(log) << "User:" << user;
        }
        QRegularExpression rePassword(m_Parameters.GetPassword(), QRegularExpression::CaseInsensitiveOption);
        match = rePassword.match(QByteArray(buf, len));
        bool bPassword = match.hasMatch();
        if(bPassword) {
            QString password = m_Parameters.m_Net.m_User.GetPassword();
            if(!password.isEmpty()) {
                m_Telnet.sendData(password.toStdString().c_str(), password.length());
                m_Telnet.sendData("\n", 1);
                m_bLogin = true;
            }
            qDebug(log) << "Password:" << password;
        }
    }
}

