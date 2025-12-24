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
    if(!m_Para->GetAnonymousLogin()) {
        szUser = m_Para->GetUser();
        szPassword = m_Para->GetPassword();
    }
    m_pServer = new FtpServer(this, m_Para->GetRoot(), m_Para->GetPort(),
                              szUser, szPassword,
                              m_Para->GetReadOnly(), false);
    m_pServer->SetFilter(this);
    if(m_pServer->isListening()) {
        qInfo(log) << "The ftp server listen in" << m_Para->GetPort();
    } else {
        QString szErr = tr("The ftp server is not listening in %1").arg(m_Para->GetPort());
        emit sigError(-1, szErr);
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
    if(!socket) return true;
    if(m_Para->GetConnectCount() >= 0 && m_Para->GetConnectCount() <= m_Sockets.size()) {
        qDebug(log) << "Exceeded the allowed number of connections:" << m_Para->GetConnectCount();
        return true;
    }

    bool check = false;
    check = connect(socket, SIGNAL(disconnected()),
                    this, SLOT(slotDisconnected()));
    Q_ASSERT(check);
    QString ip = socket->peerAddress().toString();
    quint16 port = socket->peerPort();
    m_Sockets.append(socket);
    m_nTotal++;
    emit sigConnectCount(m_nTotal, m_Sockets.size(), m_nDisconnect);
    emit sigConnected(ip, port);
    qDebug(log) << "Current connect count:" << m_Sockets.size()
             << "; new connect from:" << ip + ":" + QString::number(port);
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
