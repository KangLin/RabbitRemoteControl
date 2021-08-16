// Author: Kang Lin <kl222@126.com>

#include "Connection.h"
#include "QSocketInStream.h"
#include "QSocketOutStream.h"
#include "RabbitCommonLog.h"
#include "rfb/SSecurityVncAuth.h"
#include "rfb/SMsgWriter.h"
#include "rfb/screenTypes.h"
#include "rfb/fenceTypes.h"
#include "RabbitCommonTools.h"
#include <fstream>
#include "InputDevice.h"
#include <QScreen>
#include <QApplication>
#include "Screen.h"

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

CConnection::CConnection(QTcpSocket *pSocket,
                         CParameterServiceTigerVNC *pPara)
    : QObject(), rfb::SConnection(),
      m_DataChannel(pSocket),
      m_PixelFormat(32, 24, false, true, 255, 255, 255, 16, 8, 0),
      inProcessMessages(false),
      pendingSyncFence(false),
      syncFence(false),
      fenceFlags(0),
      fenceDataLen(0),
      fenceData(nullptr),
      m_EncodeManager(this),
      continuousUpdates(false)
{
    bool check = false;
    m_pPara = pPara;
    setStreams(m_DataChannel.InStream(), m_DataChannel.OutStream());
    if(!g_setfile) 
    {
        g_setfile = true;
        setfile();
    }
    
    char* pass = new char[128];
    strcpy(pass, pPara->getPassword().toStdString().c_str());
    rfb::PlainPasswd password(pass);
    rfb::ObfuscatedPasswd oPassword(password);
    rfb::SSecurityVncAuth::vncAuthPasswd.setParam(oPassword.buf, oPassword.length);
    
    check = connect(CScreen::Instance(), SIGNAL(sigUpdate(QImage)),
                    this, SLOT(slotScreenUpdate(QImage)));
    Q_ASSERT(check);
    client.setDimensions(CScreen::Instance()->Width(), CScreen::Instance()->Height());

    check = connect(&m_DataChannel, SIGNAL(readyRead()),
                         this, SLOT(slotReadyRead()));
    Q_ASSERT(check);
    check = connect(&m_DataChannel, SIGNAL(sigDisconnected()),
                         this, SLOT(slotDisconnected()));
    Q_ASSERT(check);
    check = connect(&m_DataChannel, SIGNAL(sigError(int, QString)),
                    this, SLOT(slotError(int, QString)));
    Q_ASSERT(check);
    
    try{
        initialiseProtocol();
    } catch(rdr::Exception& e) {
        LOG_MODEL_ERROR("Connection", "initialistProtocol() fail");
        emit sigError(-1, "initialistProtocol() fail");
    }
}

CConnection::~CConnection()
{
    LOG_MODEL_DEBUG("CConnection", "CConnection::~CConnection");
}

void CConnection::slotReadyRead()
{
    //LOG_MODEL_DEBUG("Connection", "CConnection::slotReadyRead()");
    if (state() == RFBSTATE_CLOSING) return;
    try {
        inProcessMessages = true;
        
        // Get the underlying transport to build large packets if we send
        // multiple small responses.
        getOutStream()->cork(true);
        
        while (true) {
            if (pendingSyncFence)
                syncFence = true;
            
            if (!processMsg())
                break;
            
            if (syncFence) {
                writer()->writeFence(fenceFlags, fenceDataLen, fenceData);
                syncFence = false;
                pendingSyncFence = false;
            }
        }
        
        // Flush out everything in case we go idle after this.
        getOutStream()->cork(false);
        
        inProcessMessages = false;
        
        // If there were anything requiring an update, try to send it here.
        // We wait until now with this to aggregate responses and to give 
        // higher priority to user actions such as keyboard and pointer events.
        if(writer())
        {
            QImage img = CScreen::Instance()->GetScreen();
            slotScreenUpdate(img);
        }
    } catch (rdr::Exception e) {
        LOG_MODEL_ERROR("CConnection", "Exception:%s", e.str());
        emit sigError(-1, QString("processMsg exception:") + e.str());
    } catch (std::exception e) {
        LOG_MODEL_ERROR("CConnection", "std Exception:%s", e.what());
        emit sigError(-1, QString("std Exception:") + e.what());
    } catch (...) {
        LOG_MODEL_ERROR("CConnection", "exception");
        emit sigError(-1, QString("Exception:"));
    }
}

void CConnection::slotDisconnected()
{
    //TODO: close and clean
    LOG_MODEL_DEBUG("Connection", "The connect disconnect");
    emit sigDisconnected();
}

void CConnection::slotError(int nErr, QString szErr)
{
    emit sigError(nErr, szErr);
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
    // Set the connection parameters appropriately
    QString name = RabbitCommon::CTools::GetHostName()
            + "@" + RabbitCommon::CTools::GetCurrentUser();
    client.setName(name.toStdString().c_str());
    
    //TODO: add client.setLEDState
    // client.setLEDState
    
    int w = CScreen::Instance()->Width();
    int h = CScreen::Instance()->Height();
    client.setDimensions(w, h);
    client.setPF(m_PixelFormat);
    char buffer[256];
    client.pf().print(buffer, 256);
    LOG_MODEL_INFO("Connection", "Set server pixel format:%s; width:%d, height:%d",
                   buffer, w, h);

    // Mark the entire display as "dirty"
    m_Updates.add_changed(rfb::Rect(0, 0, w, w));
}

void CConnection::clientInit(bool shared)
{
    LOG_MODEL_DEBUG("Connection", "clientInit shared:%d", shared);
    SConnection::clientInit(shared);
}

void CConnection::setDesktopSize(int fb_width, int fb_height, const rfb::ScreenSet &layout)
{
    LOG_MODEL_DEBUG("Connection", "setDesktopSize: %d:%d", fb_width, fb_height);

    //TODO: Add set server desktop size
    if(writer())
        writer()->writeDesktopSize(rfb::reasonClient, rfb::resultSuccess);
}

void CConnection::setPixelFormat(const rfb::PixelFormat &pf)
{
    SConnection::setPixelFormat(pf);
    char buffer[256];
    pf.print(buffer, 256);
    LOG_MODEL_DEBUG("CConnection", "Set pixel format: %s", buffer);
    
    //TODO: add setCursor();

}

void CConnection::framebufferUpdateRequest(const rfb::Rect &r, bool incremental)
{
    LOG_MODEL_DEBUG("Connection", "framebufferUpdateRequest:incremental: %d; %d,%d,%d,%d",
                    incremental, r.tl.x, r.tl.y,
                    r.br.x, r.br.y);
    if (!accessCheck(AccessView)) return;
    
    SConnection::framebufferUpdateRequest(r, incremental);
    
    rfb::Rect safeRect;
    // Check that the client isn't sending crappy requests
    if (!r.enclosed_by(rfb::Rect(0, 0, client.width(), client.height()))) {
        LOG_MODEL_ERROR("Connection", "FramebufferUpdateRequest %dx%d at %d,%d exceeds framebuffer %dx%d",
                   r.width(), r.height(), r.tl.x, r.tl.y,
                   client.width(), client.height());
        safeRect = r.intersect(rfb::Rect(0, 0, client.width(), client.height()));
    } else {
        safeRect = r;
    }
    
    // Just update the requested region.
    // Framebuffer update will be sent a bit later, see processMessages().
    rfb::Region reqRgn(safeRect);
    if (!incremental || !continuousUpdates)
        requested.assign_union(reqRgn);
    
    if (!incremental) {
        // Non-incremental update - treat as if area requested has changed
        m_Updates.add_changed(reqRgn);
        
        // And send the screen layout to the client (which, unlike the
        // framebuffer dimensions, the client doesn't get during init)
        if (client.supportsEncoding(rfb::pseudoEncodingExtendedDesktopSize))
            writer()->writeDesktopSize(rfb::reasonServer);
        
        // We do not send a DesktopSize since it only contains the
        // framebuffer size (which the client already should know) and
        // because some clients don't handle extra DesktopSize events
        // very well.
    }
}

void CConnection::fence(rdr::U32 flags, unsigned len, const char data[])
{
    LOG_MODEL_DEBUG("Connection", "fence: flags:%d,len:%d", flags, len);
    rdr::U8 type;
    
    if (flags & rfb::fenceFlagRequest) {
        if (flags & rfb::fenceFlagSyncNext) {
            pendingSyncFence = true;
            
            fenceFlags = flags & (rfb::fenceFlagBlockBefore | rfb::fenceFlagBlockAfter | rfb::fenceFlagSyncNext);
            fenceDataLen = len;
            delete [] fenceData;
            fenceData = NULL;
            if (len > 0) {
                fenceData = new char[len];
                memcpy(fenceData, data, len);
            }
            
            return;
        }
        
        // We handle everything synchronously so we trivially honor these modes
        flags = flags & (rfb::fenceFlagBlockBefore | rfb::fenceFlagBlockAfter);
        if(writer())
            writer()->writeFence(flags, len, data);
        return;
    }
    
    if (len < 1)
        LOG_MODEL_ERROR("Connection", "Fence response of unexpected size received");
    
    type = data[0];
    
    switch (type) {
    case 0:
        // Initial dummy fence;
        break;
    case 1:
        //congestion.gotPong();
        break;
    default:
        LOG_MODEL_ERROR("Connection", "Fence response of unexpected type received");
    }
}

void CConnection::enableContinuousUpdates(bool enable, int x, int y, int w, int h)
{
    LOG_MODEL_DEBUG("Connection", "enableContinuousUpdates: %d, %d,%d,%d,%d",
                    enable, x,y,w,h);
    //SConnection::enableContinuousUpdates(enable, x, y, w, h);
    
    rfb::Rect rect;
  
    if (!client.supportsFence() || !client.supportsContinuousUpdates())
      throw rdr::Exception("Client tried to enable continuous updates when not allowed");
  
    continuousUpdates = enable;
  
    rect.setXYWH(x, y, w, h);
    cuRegion.reset(rect);
  
    if (enable) {
      requested.clear();
    } else {
      writer()->writeEndOfContinuousUpdates();
    }
    
}

void CConnection::keyEvent(rdr::U32 keysym, rdr::U32 keycode, bool down)
{
    LOG_MODEL_DEBUG("Connection", "keysym:%d;keycode:%d;down:%d", keysym, keycode, down);
    m_InputDevice.KeyEvent(keysym, keycode, down);
}

void CConnection::pointerEvent(const rfb::Point &pos, int buttonMask)
{
    //LOG_MODEL_DEBUG("Connection", "pos:%d,%d;button:%d", pos.x, pos.y, buttonMask);
    CInputDevice::MouseButtons button;
    if(buttonMask & 0x1)
        button |= CInputDevice::LeftButton;
    if(buttonMask & 0x2)
        button |= CInputDevice::MouseButton::MiddleButton;
    if(buttonMask & 0x4)
        button |= CInputDevice::MouseButton::RightButton;
    if(buttonMask & 0x8)
        button |= CInputDevice::MouseButton::UWheelButton;
    if(buttonMask & 0x10)
        button |= CInputDevice::MouseButton::DWheelButton;
    if(buttonMask & 0x20)
        button |= CInputDevice::MouseButton::LWheelButton;
    if(buttonMask & 0x40)
        button |= CInputDevice::MouseButton::RWheelButton;
    
    m_InputDevice.MouseEvent(button, QPoint(pos.x, pos.y));
}

void CConnection::clientCutText(const char *str)
{
    LOG_MODEL_DEBUG("Connection", "cut text:%s", str);
}


QSharedPointer<rfb::PixelBuffer> CConnection::GetBufferFromQImage(QImage &img)
{
    if(img.isNull())
        return nullptr;

    QSharedPointer<rfb::PixelBuffer> pb(
                new rfb::FullFramePixelBuffer(m_PixelFormat,
                                              img.width(),
                                              img.height(),
                                              img.bits(),
                                              img.bytesPerLine()
                                              / (m_PixelFormat.bpp / 8)));
    return pb;
}

void CConnection::writeNoDataUpdate()
{
//  if (!writer()->needNoDataUpdate())
//    return;

  writer()->writeNoDataUpdate();

  // Make sure no data update is sent until next request
  //requested.clear();
}

void CConnection::writeDataUpdate(QImage img)
{
    rfb::Region req;
    rfb::UpdateInfo ui;
    const rfb::RenderedCursor *cursor = nullptr;
    
    // See what the client has requested (if anything)
    if (continuousUpdates)
      req = cuRegion.union_(requested);
    else
      req = requested;
  
    if (req.is_empty())
      return;
    
    if(img.isNull())
        return;

    // Get the lists of updates. Prior to exporting the data to the `ui' object,
    // getUpdateInfo() will normalize the `updates' object such way that its
    // `changed' and `copied' regions would not intersect.
    m_Updates.getUpdateInfo(&ui, req);

    QSharedPointer<rfb::PixelBuffer> buffer = GetBufferFromQImage(img);
    m_EncodeManager.writeUpdate(ui, buffer.get(), cursor);

}

void CConnection::slotScreenUpdate(QImage img)
{
    LOG_MODEL_DEBUG("Connection", "Update screen");
    if(img.isNull())
    {
        LOG_MODEL_ERROR("Connection", "Image is null");
        return;
    }
    
    writeDataUpdate(img);
}
