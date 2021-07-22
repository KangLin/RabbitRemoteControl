#include "Connection.h"
#include "Viewer/Plugins/TigerVnc/QSocketInStream.h"
#include "Viewer/Plugins/TigerVnc/QSocketOutStream.h"
#include "RabbitCommonLog.h"
#include "rfb/SSecurityVncAuth.h"
#include "RabbitCommonTools.h"
#include <fstream>

static void setfile()
{
    char* file = "/home/build-RabbitRemoteControl-Desktop_Qt_5_12_11_GCC_64bit-Debug/password";
    char* pPass = new char[9];
    strcpy(pPass, "123456");
    rfb::PlainPasswd password(pPass);
    rfb::ObfuscatedPasswd oPassword(password);
    std::ofstream out;
    out.open(file);
    if(out.is_open())
    {
        out.write(oPassword.takeBuf(), oPassword.length);
        out.close();
    }
    rfb::SSecurityVncAuth::vncAuthPasswdFile.setParam(file);
}
bool g_setfile = false;

CConnection::CConnection(QTcpSocket *pSocket) : QObject(), rfb::SConnection()
{
    m_pSocket = pSocket;
    setStreams(new CQSocketInStream(pSocket), new CQSocketOutStream(pSocket));
    if(!g_setfile) 
    {
        g_setfile = true;
        setfile();
    }
    char* pPass = new char[9];
    strcpy(pPass, "123456");
    rfb::PlainPasswd password(pPass);
    rfb::ObfuscatedPasswd oPassword(password);
    rfb::SSecurityVncAuth::vncAuthPasswd.setParam(oPassword.buf, oPassword.length);
    
    client.setDimensions(640, 480);
    initialiseProtocol();
    
    bool check = connect(m_pSocket, SIGNAL(readyRead()),
                         this, SLOT(slotReadyRead()));
    Q_ASSERT(check);
}

CConnection::~CConnection()
{
    if(getInStream())
        delete this->getInStream();
    if(getOutStream())
        delete getOutStream();
    if(m_pSocket)
        m_pSocket->deleteLater();
}

void CConnection::setDesktopSize(int fb_width, int fb_height, const rfb::ScreenSet &layout)
{
    LOG_MODEL_DEBUG("Connection", "setDesktopSize: %d:%d", fb_width, fb_height);
    client.setDimensions(fb_width, fb_height, layout);
}

void CConnection::slotReadyRead()
{
    //LOG_MODEL_DEBUG("Connection", "CConnection::slotReadyRead()");
    try {
        while(processMsg())
        {
            ;
        }        
    } catch (rdr::Exception e) {
        LOG_MODEL_ERROR("CConnection", "Exception:%s", e.str());
    } catch (std::exception e) {
        LOG_MODEL_ERROR("CConnection", "std Exception:%s", e.what());
    } catch (...) {
        LOG_MODEL_ERROR("CConnection", "exception");
    }
}

void CConnection::queryConnection(const char *userName)
{
    LOG_MODEL_DEBUG("Connection", "queryConnection: %s", userName);
    //TODO: check user is accpet?
    SConnection::queryConnection(userName);
}

void CConnection::authSuccess()
{
    LOG_MODEL_DEBUG("Connection", "CConnection::authSuccess(): state:%d", state());
}

void CConnection::clientInit(bool shared)
{
    LOG_MODEL_DEBUG("Connection", "clientInit shared:%d", shared);
    QString name = RabbitCommon::CTools::GetHostName()
            + "@" + RabbitCommon::CTools::GetCurrentUser();
    
    client.setName(name.toStdString().c_str());
    SConnection::clientInit(shared);
}

void CConnection::keyEvent(rdr::U32 keysym, rdr::U32 keycode, bool down)
{
    LOG_MODEL_DEBUG("Connection", "keysym:%d;keycode:%d;down:%d", keysym, keycode, down);
}

void CConnection::pointerEvent(const rfb::Point &pos, int buttonMask)
{
    //LOG_MODEL_DEBUG("Connection", "pos:%d,%d;button:%d", pos.x, pos.y, buttonMask);
}

void CConnection::clientCutText(const char *str)
{
    LOG_MODEL_DEBUG("Connection", "cut text:%s", str);
}
