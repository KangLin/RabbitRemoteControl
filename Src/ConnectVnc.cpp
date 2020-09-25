#include "ConnectVnc.h"
#include "rfb/LogWriter.h"
#include "rfb/secTypes.h"
#include "rfb/CSecurityNone.h"
#include "rfb/CSecurityVncAuth.h"
#include "rfb/Exception.h"

static rfb::LogWriter vlog("ConnectVnc");

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

    try {
        setShared(GetShared());
        addSecType(rfb::secTypeNone);
        addSecType(rfb::secTypeVncAuth);
        cp.supportsDesktopResize = GetReDesktopSize();
        cp.supportsLocalCursor = GetUserLocalCursor();
        
//        rfb::CharArray encStr(preferredEncoding.getData());
//        int encNum = rfb::encodingNum(encStr.buf);
//        if (encNum != -1) {
//          currentEncoding = encNum;
//          autoSelect = false;
//        }
        
        
        m_pSock = new network::TcpSocket(m_szIp.toStdString().c_str(), m_nPort);
        vlog.info("Connecting to %s:%d", m_szIp.toStdString().c_str(), m_nPort);
        m_pSock->inStream().setBlockCallback(this);
        setServerName(m_pSock->getPeerEndpoint());
        setStreams(&m_pSock->inStream(), &m_pSock->outStream());
        initialiseProtocol();
    } catch (rdr::EndOfStream& e) {
        vlog.info(e.str());
    } catch (rdr::Exception& e) {
        vlog.error(e.str());
    }
    
    return nRet;
}

int CConnectVnc::Exec()
{
    int nRet = 0;
    
    try {
        while (!m_bExit) {
            getInStream()->check(1);
            processMsg();    
        }
    } catch (rdr::EndOfStream& e) {
        
        vlog.info(e.str());
    } catch (rdr::Exception& e) {
        vlog.error(e.str());
        nRet = -1;
    }
    
    emit sigDisconnect();
    
    return nRet;
}

void CConnectVnc::blockCallback()
{}

void CConnectVnc::getUserPasswd(char **user, char **password)
{
    if(user)
        *user = rfb::strDup(m_szUser.toStdString().c_str());
    if(password)
        *password = rfb::strDup(m_szPassword.toStdString().c_str());
}

rfb::CSecurity* CConnectVnc::getCSecurity(int secType)
{
    switch (secType) {
    case rfb::secTypeNone:
      return new rfb::CSecurityNone();
    case rfb::secTypeVncAuth:
      return new rfb::CSecurityVncAuth(this);
    default:
      throw rfb::Exception("Unsupported secType?");
    }
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

void CConnectVnc::setColourMapEntries(int firstColour, int nColours, rdr::U16 *rgbs)
{
    //TODO:设置调色板
    
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
