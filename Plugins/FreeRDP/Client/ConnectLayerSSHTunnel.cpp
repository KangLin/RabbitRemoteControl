// Author: Kang Lin <kl222@126.com>

#include "winpr/winsock.h"
#include "ConnectLayerSSHTunnel.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "FreeRDP.Connect.Layer.SSH")

ConnectLayerSSHTunnel::ConnectLayerSSHTunnel(CBackendFreeRDP *connect)
    : CConnectLayer{connect}
    , m_pChannelSSH(nullptr)
    , m_hSshSocket(nullptr)
{}

ConnectLayerSSHTunnel::~ConnectLayerSSHTunnel()
{
    qDebug(log) << Q_FUNC_INFO;
}

int ConnectLayerSSHTunnel::OnInit(rdpContext *context)
{
    int nRet = 0;

    m_pChannelSSH = new CChannelSSHTunnel(
        &m_pParameter->m_Proxy.m_SSH, &m_pParameter->m_Net, m_pOperate);
    if(!m_pChannelSSH)
        return -1;
    bool bRet = m_pChannelSSH->open(QIODevice::ReadWrite);
    if(!bRet)
        return -1;
    if(SSH_INVALID_SOCKET == m_pChannelSSH->GetSocket()) {
        qCritical(log) << "The socket is invalid";
        return -1;
    }
    m_hSshSocket = WSACreateEvent();
    if(!m_hSshSocket) {
        qCritical(log) << "CreateEvent ssh socket event failed";
        return -1;
    }
    nRet = WSAEventSelect(m_pChannelSSH->GetSocket(), m_hSshSocket, FD_READ | FD_CLOSE);
    if(nRet)
        return -1;
    if(m_pParameter->m_Net.GetHost().isEmpty())
    {
        QString szErr;
        szErr = tr("The server is empty, please input it");
        qCritical(log) << szErr;
        emit m_pOperate->sigShowMessageBox(tr("Error"), szErr, QMessageBox::Critical);
        emit m_pOperate->sigError(-1, szErr.toStdString().c_str());
        return -1;
    }

    auto &net = m_pParameter->m_Net;
    auto settings = context->settings;
    freerdp_settings_set_string(
        settings, FreeRDP_ServerHostname,
        net.GetHost().toStdString().c_str());
    freerdp_settings_set_uint32(
        settings, FreeRDP_ServerPort,
        net.GetPort());

    nRet = freerdp_client_start(context);
    if(nRet)
    {
        qCritical(log) << "freerdp_client_start fail";
        return -1;
    }
    return 0;
}

int ConnectLayerSSHTunnel::OnClean()
{
    int nRet = 0;

    if(m_pChannelSSH)
    {
        m_pChannelSSH->close();
        m_pChannelSSH->deleteLater();
        m_pChannelSSH = nullptr;
    }
    if(m_hSshSocket)
    {
        WSACloseEvent(m_hSshSocket);
        m_hSshSocket = nullptr;
    }

    return nRet;
}

int ConnectLayerSSHTunnel::OnLayerRead(void *data, int bytes)
{
    int nRet = 0;

    if(!m_pChannelSSH || !m_pChannelSSH->isOpen()) {
        qCritical(log) << Q_FUNC_INFO << "The channel is close";
        return -1;
    }

    if(m_hSshSocket)
        WSAResetEvent(m_hSshSocket);

    nRet = m_pChannelSSH->read((char*)data, bytes);

    return nRet;
}

int ConnectLayerSSHTunnel::OnLayerWrite(const void *data, int bytes)
{
    //qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    if(!m_pChannelSSH || !m_pChannelSSH->isOpen()) {
        qCritical(log) << Q_FUNC_INFO << "The channel is close";
        return -1;
    }
    nRet = m_pChannelSSH->write((const char*)data, bytes);
    return nRet;
}

BOOL ConnectLayerSSHTunnel::OnLayerWait(BOOL waitWrite, DWORD timeout)
{
    //qDebug(log) << Q_FUNC_INFO << "wait write:" << waitWrite;
    Q_ASSERT(!waitWrite);
    if(!m_pChannelSSH || !m_pChannelSSH->isOpen()
        || m_pChannelSSH->GetSocket() == SSH_INVALID_SOCKET) {
        qCritical(log) << Q_FUNC_INFO << "The channel is close";
        return false;
    }

    int nRet = m_pChannelSSH->DoWait(waitWrite, timeout);
    if(nRet) return false;
    return true;
}

HANDLE ConnectLayerSSHTunnel::OnLayerGetEvent()
{
    //qDebug(log) << Q_FUNC_INFO;
    return m_hSshSocket;
}
