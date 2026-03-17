// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QSslSocket>
#include <QLoggingCategory>
#include "BackendFtpServer.h"
#include "OperateFtpServer.h"

static Q_LOGGING_CATEGORY(log, "FtpServer.Backend")
CBackendFtpServer::CBackendFtpServer(COperate *pOperate) : CBackend(pOperate)
    , m_pServer(nullptr)
    , m_nTotal(0)
    , m_nDisconnect(0)
{
    qDebug(log) << Q_FUNC_INFO;
    COperateFtpServer* po = qobject_cast<COperateFtpServer*>(pOperate);
    m_Para = po->GetParameter();
}

CBackendFtpServer::~CBackendFtpServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

CBackend::OnInitReturnValue CBackendFtpServer::OnInit()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pServer) {
        qCritical(log) << "Server is exist";
        return OnInitReturnValue::Fail;
    }

    m_nTotal = 0;
    m_nDisconnect = 0;
    m_Sockets.clear();
    emit sigConnectCount(m_nTotal, m_Sockets.size(), m_nDisconnect);

    QString szUser;
    QString szPassword;
    auto &net = m_Para->m_Net;
    if(!m_Para->GetAnonymousLogin()) {
        auto &user = net.m_User;
        szUser = user.GetName();
        szPassword = user.GetPassword();
    }

    m_pServer = new CFtpServer(this, m_Para->GetRoot(), net.GetPort(),
                              szUser, szPassword,
                              m_Para->GetReadOnly());
    if(!m_pServer) {
        qCritical(log) << "Failed to new CFtpServer";
        return OnInitReturnValue::Fail;
    }

    m_pServer->SetFilter(this);
    bool bListen = false;
    if(m_Para->GetListenAll()) {
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
        if(m_Para->GetListen().isEmpty()) {
            szErr = tr("Failed: Ftp server is not set to listen on any address");
            qCritical(log) << szErr;
            emit sigError(-1, szErr);
            return OnInitReturnValue::Fail;
        }
        foreach (auto a, m_Para->GetListen()) {
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
    
    QStringList white = m_Para->GetWhitelist();
    QStringList black = m_Para->GetBlacklist();
    bool bInWhite = false;
    if(!white.isEmpty()) {
        foreach(auto i, white) {
            QHostAddress addr(szIP);
            auto sub = QHostAddress::parseSubnet(i);
            if(addr.isInSubnet(sub.first, sub.second)) {
                bInWhite = true;
                break;
            }
        }
    }

    if(!bInWhite && !black.isEmpty()) {
        foreach(auto i, black) {
            QHostAddress addr(szIP);
            auto sub = QHostAddress::parseSubnet(i);
            if(addr.isInSubnet(sub.first, sub.second)) {
                qInfo(log) << "Filet" << szIP << "in blacklist";
                return true;
            }
        }
    }

    if(bFilte) return true;

    if(m_Para->GetConnectCount() >= 0 && m_Para->GetConnectCount() <= m_Sockets.size()) {
        qDebug(log) << "Exceeded the allowed number of connections:" << m_Para->GetConnectCount();
        return true;
    }

    bool check = false;
    check = connect(socket, SIGNAL(disconnected()),
                    this, SLOT(slotDisconnected()));
    Q_ASSERT(check);
    
    quint16 port = socket->peerPort();
    m_Sockets.append(socket);
    m_nTotal++;
    emit sigConnectCount(m_nTotal, m_Sockets.size(), m_nDisconnect);
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
    m_nDisconnect++;
    emit sigConnectCount(m_nTotal, m_Sockets.size(), m_nDisconnect);
    QString ip = socket->peerAddress().toString();
    quint16 port = socket->peerPort();
    emit sigDisconnected(ip, port);
    qDebug(log) << "Current connect count:" << m_Sockets.size()
             << "; remove connect:" << socket->peerAddress().toString();
}

void CBackendFtpServer::slotDisconnect(const QString& szIp, quint16 port)
{
    foreach (auto s, m_Sockets) {
        if(s->peerAddress().toString() == szIp && s->peerPort() == port){
            s->disconnectFromHost();
        }
    }
}
