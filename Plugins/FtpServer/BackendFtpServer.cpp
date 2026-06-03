// Author: Kang Lin <kl222@126.com>

#include <QSslSocket>
#include <QLoggingCategory>
#include "BackendFtpServer.h"
#include "OperateFtpServer.h"
#include "ParameterFtpServer.h"
#include "Backend.h"

static Q_LOGGING_CATEGORY(log, "Backend.FtpServer")
CBackendFtpServer::CBackendFtpServer(COperateFtpServer* pOperate, bool bStopSignal)
    : CBackendServer(pOperate, bStopSignal)
    , m_pOperate(pOperate)
    , m_pServer(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    m_pPara = m_pOperate->GetParameter();
}

CBackendFtpServer::~CBackendFtpServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

/*!
 * \~chinese 初始化
 * \return
 * \li OnInitReturnValue::Fail: 错误
 * \li OnInitReturnValue::Success/OnInitReturnValue::UseOnProcess: 使用 OnProcess() (非 Qt 事件循环)
 * \li OnInitReturnValue::NotUseOnProcess: 不使用 OnProcess() (qt 事件循环)
 *
 * \~english Initialization
 * \return CBackend::OnInitReturnValue
 * \li OnInitReturnValue::Fail: error
 * \li OnInitReturnValue::Success/OnInitReturnValue::UseOnProcess: Use OnProcess() (non-Qt event loop)
 * \li OnInitReturnValue::NotUseOnProcess: Don't use OnProcess() (qt event loop)
 *
 * \~
 * \see Start()
 */
CBackend::OnInitReturnValue CBackendFtpServer::OnInit()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pServer) {
        qCritical(log) << "Server is exist";
        return OnInitReturnValue::Fail;
    }

    m_Sockets.clear();

    CSecurityLevel::Levels securityLevel;
    QString szUser;
    QString szPassword;
    auto &net = m_pPara->m_Net;
    if(!m_pPara->GetAnonymousLogin()) {
        auto &user = net.m_User;
        szUser = user.GetUser();
        szPassword = user.GetPassword();
        if(!szPassword.isEmpty())
            securityLevel |= CSecurityLevel::Level::Authentication;
    }

    m_pServer = new CFtpServer(this, m_pPara->GetRoot(), net.GetPort(),
                               szUser, szPassword,
                               m_pPara->GetReadOnly());
    if(!m_pServer) {
        qCritical(log) << "Failed to new CFtpServer";
        return OnInitReturnValue::Fail;
    }

    m_pServer->SetFilter(this);
    bool bListen = false;
    if(m_pPara->GetListenAll()) {
        bListen = m_pServer->Listening();
        if(bListen) {
            QString szMsg = tr("Ftp server listen on all address port %1. the lan ip is %2")
            .arg(net.GetPort()).arg(m_pServer->lanIp());
            qInfo(log) << szMsg;
            emit sigInformation(szMsg);
        } else {
            QString szErr = tr("Failed to Ftp server is listening on %1")
            .arg(net.GetPort());
            qCritical(log) << szErr;
            emit sigError(-1, szErr);
            return OnInitReturnValue::Fail;
        }
    } else {
        QString szErr;
        if(m_pPara->GetListen().isEmpty()) {
            szErr = tr("Failed: Ftp server is not set to listen on any address");
            qCritical(log) << szErr;
            emit sigError(-1, szErr);
            return OnInitReturnValue::Fail;
        }
        foreach (auto a, m_pPara->GetListen()) {
            QHostAddress addr(a);
            bListen = m_pServer->Listening(addr);
            if(!bListen) {
                szErr = tr("Failed to Ftp server is listening on %1:%2")
                .arg(addr.toString()).arg(net.GetPort());
                qCritical(log) << szErr;
                emit sigError(-1, szErr);
                return OnInitReturnValue::Fail;
            }
            if(!szErr.isEmpty())
                szErr += "; ";
            szErr += addr.toString() + ":" + QString::number(net.GetPort());
        }
        if(bListen) {
            QString szMsg = tr("Ftp server is listening on ") + szErr;
            qInfo(log) << szMsg;
            emit sigInformation(szMsg);
        }
    }

    emit sigSecurityLevel(securityLevel);
    return OnInitReturnValue::NotUseOnProcess;
}

int CBackendFtpServer::OnClean()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pServer)
        delete m_pServer;
    return 0;
}

bool CBackendFtpServer::onFilter(QSslSocket *socket)
{
    bool bFilte = false;
    if(!socket) return true;
    QString szIP = socket->peerAddress().toString();

    auto& white = m_pPara->m_WhiteFilter;
    auto& black = m_pPara->m_BlackFilter;

    bool bInWhite = false;
    if(!white.isEmpty()) {
        white.OnProcess([&](const QString& szKey)->int {
            QHostAddress addr(szIP);
            auto sub = QHostAddress::parseSubnet(szKey);
            if(addr.isInSubnet(sub.first, sub.second)) {
                bInWhite = true;
                return -1;
            }
            return 0;
        }, true);
    }

    if(!bInWhite && !black.isEmpty()) {
        black.OnProcess([&](const QString& szKey)->int {
            QHostAddress addr(szIP);
            auto sub = QHostAddress::parseSubnet(szKey);
            if(addr.isInSubnet(sub.first, sub.second)) {
                qInfo(log) << "Filtered" << szIP << "in blacklist";
                bFilte = true;
                return -1;
            }
            return 0;
        }, true);
    }

    if(bFilte) return true;

    if(m_pPara->GetConnectCount() >= 0 && m_pPara->GetConnectCount() <= m_Sockets.size()) {
        qDebug(log) << "Exceeded the allowed number of connections:" << m_pPara->GetConnectCount();
        return true;
    }

    bool check = false;
    check = connect(socket, SIGNAL(disconnected()),
                    this, SLOT(slotDisconnected()));
    Q_ASSERT(check);

    quint16 port = socket->peerPort();
    m_Sockets.append(socket);
    emit sigConnected(szIP, port);
    qDebug(log) << "Current connect count:" << m_Sockets.size()
                << "; new connect from:" << szIP + ":" + QString::number(port);
    return false;
}

void CBackendFtpServer::slotDisconnected()
{
    QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
    if(!socket) return;
    m_Sockets.removeAll(socket);
    QString ip = socket->peerAddress().toString();
    quint16 port = socket->peerPort();
    emit sigDisconnected(ip, port);
    qDebug(log) << "Current connect count:" << m_Sockets.size()
                << "; remove connect:" << socket->peerAddress().toString();
}

void CBackendFtpServer::slotDisconnect(const QString &szIp, const quint16 port)
{
    foreach (auto s, m_Sockets) {
        if(s->peerAddress().toString() == szIp && s->peerPort() == port){
            s->disconnectFromHost();
        }
    }
}
