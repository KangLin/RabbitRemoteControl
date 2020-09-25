#include "ConnectVnc.h"
#include "rfb/LogWriter.h"

rfb::LogWriter vlog("ConnectVnc");

CConnectVnc::CConnectVnc(CFrmViewer *pView, QObject *parent)
    : CConnect(pView, parent)
{
    m_pSock = nullptr;
}

CConnectVnc::~CConnectVnc()
{
    if(m_pSock) delete m_pSock;
}

int CConnectVnc::Connect()
{
    int nRet = 0;
    if(m_pSock) delete m_pSock;
    
    m_pSock = new network::TcpSocket(m_szIp.toStdString().c_str(), m_nPort);
    m_pSock->inStream().setBlockCallback(this);
    setServerName(m_pSock->getPeerEndpoint());
    setStreams(&m_pSock->inStream(), &m_pSock->outStream());
    initialiseProtocol();
    return nRet;
}

int CConnectVnc::Exec()
{
    try {
        while (!m_bExit) {
            getInStream()->check(1);
            processMsg();    
        }
    }  catch (rdr::EndOfStream& e) {
        vlog.info(e.str());
    } catch (rdr::Exception& e) {
        vlog.error(e.str());
        return -1;
    }
    
    return 0;
}

void CConnectVnc::blockCallback()
{}

rfb::CSecurity* CConnectVnc::getCSecurity(int secType)
{
    return nullptr;
}

void CConnectVnc::serverInit()
{
    rfb::CConnection::serverInit();
    emit sigConnected();
}

void CConnectVnc::setDesktopSize(int w, int h)
{
    emit sigSetDesktopSize(w, h);
}

void CConnectVnc::bell()
{
    
}

void CConnectVnc::serverCutText(const char *str, int len)
{
    QString szText = QString::fromLatin1(str, len);
    emit sigServerCutText(szText);
}

QString CConnectVnc::GetServerName()
{
    return getServerName();
}
