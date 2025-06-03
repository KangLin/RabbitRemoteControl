#include "winpr/winsock.h"
#include "ConnectLayerQTcpSocket.h"
#include <QNetworkProxy>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "FreeRDP.Connect.Layer.QTcpSocket")

CConnectLayerQTcpSocket::CConnectLayerQTcpSocket(CConnectFreeRDP *connect)
    : CConnectLayer(connect)
    , m_hSocket(nullptr)
    , m_TcpSocket(this)
{
    qDebug(log) << Q_FUNC_INFO;
}

CConnectLayerQTcpSocket::~CConnectLayerQTcpSocket()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CConnectLayerQTcpSocket::slotError(QAbstractSocket::SocketError e)
{
    QString szError;
    szError = m_TcpSocket.errorString();
    qDebug(log) << "CConnectLayerQTcpSocket::slotError()" << e << szError;
    emit m_pConnect->sigError(e, szError);
}

void CConnectLayerQTcpSocket::slotConnected()
{
    int nRet = 0;
    
    //*
    if(m_hSocket) {
        nRet = WSAEventSelect(m_TcpSocket.socketDescriptor(), m_hSocket, FD_READ | FD_CLOSE);
        if(nRet) {
            qCritical(log) << "WSAEventSelect fail:" << WSAGetLastError();
            return;
        }
    }//*/

    if(!m_pConnect) return;

    rdpContext* context = (rdpContext*)m_pConnect->m_pContext;
    auto settings = context->settings;
    auto &net = m_pParameter->m_Net;
    qDebug(log) << "Connected to"
                << net.GetHost() + ":" + QString::number(net.GetPort());

    if(!m_pParameter->GetDomain().isEmpty())
        freerdp_settings_set_string(
            settings, FreeRDP_Domain,
            m_pParameter->GetDomain().toStdString().c_str());
    if(net.GetHost().isEmpty())
    {
        QString szErr;
        szErr = tr("The server is empty, please input it");
        qCritical(log) << szErr;
        return;
    }

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
        emit m_pConnect->sigError(nRet, "freerdp_client_start fail");
        return;
    }

    bool check = connect(&m_TcpSocket, SIGNAL(readyRead()),
                         this, SLOT(slotReadyRead()));

    Q_ASSERT(check);
}

void CConnectLayerQTcpSocket::slotReadyRead()
{
    qDebug(log) << "readRead ......";
    int nRet = m_pConnect->OnProcess(0);
    if(-1 > nRet)
        emit m_pConnect->sigError(nRet, "Process fail");
}

int CConnectLayerQTcpSocket::OnInit(rdpContext *context)
{
    int nRet = 0;
    
    bool check = false;
    
    check = connect(&m_TcpSocket, SIGNAL(connected()),
                    this, SLOT(slotConnected()));
    Q_ASSERT(check);

    check = connect(&m_TcpSocket, SIGNAL(disconnected()),
                    m_pConnect, SIGNAL(sigDisconnect()));
    Q_ASSERT(check);

    check = connect(&m_TcpSocket,
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
                    SIGNAL(errorOccurred(QAbstractSocket::SocketError)),
#else
                    SIGNAL(error(QAbstractSocket::SocketError)),
#endif
                    this, SLOT(slotError(QAbstractSocket::SocketError)));
    Q_ASSERT(check);
    
    m_hSocket = WSACreateEvent();
    if(!m_hSocket) {
        qCritical(log) << "CreateEvent ssh socket event failed";
        return -1;
    }

    auto &net = m_pParameter->m_Net;
    m_TcpSocket.connectToHost(net.GetHost(), net.GetPort());

    return nRet;
}

int CConnectLayerQTcpSocket::OnClean()
{
    if(m_TcpSocket.isOpen())
        m_TcpSocket.close();

    if(m_hSocket)
    {
        WSACloseEvent(m_hSocket);
        m_hSocket = nullptr;
    }
    return 0;
}

int CConnectLayerQTcpSocket::OnLayerRead(void *data, int bytes)
{
    //*
     if(m_hSocket)
        WSAResetEvent(m_hSocket);//*/
    int nRet = m_TcpSocket.read((char*)data, bytes);
    qDebug(log) << Q_FUNC_INFO << nRet << bytes;
    return nRet;
}

int CConnectLayerQTcpSocket::OnLayerWrite(const void *data, int bytes)
{
    int nRet = m_TcpSocket.write((char*)data, bytes);
    qDebug(log) << Q_FUNC_INFO << nRet << bytes;
    return nRet;
}

BOOL CConnectLayerQTcpSocket::OnLayerWait(BOOL waitWrite, DWORD timeout)
{
    qDebug(log) << Q_FUNC_INFO << waitWrite << timeout;
    bool bRet = false;
    if(!m_TcpSocket.isOpen()) return false;
    if(waitWrite)
        bRet = m_TcpSocket.waitForBytesWritten(timeout);
    else
        bRet = m_TcpSocket.waitForReadyRead(timeout);
    qDebug(log) << Q_FUNC_INFO << waitWrite << timeout << bRet;
    return bRet;
}

HANDLE CConnectLayerQTcpSocket::OnLayerGetEvent()
{
    return m_hSocket;
}
