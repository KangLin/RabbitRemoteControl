// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTVNC_H
#define CCONNECTVNC_H

#pragma once

#include "Connect.h"
#include "ParameterConnecter.h"
#include "network/TcpSocket.h"
#include "rfb/CConnection.h"
#include "rfb/UserPasswdGetter.h"
#include "rfb/UserMsgBox.h"
#include "Channel.h"
#include "ParameterVnc.h"
#include "../InStreamChannel.h"
#include "../OutStreamChannel.h"
#include <QEventLoop>

class CConnecterDesktopThread;
class CConnectVnc
    : public CConnect,
      public rfb::CConnection,
      public rfb::UserPasswdGetter,
      public rfb::UserMsgBox
{
    Q_OBJECT

public:
    explicit CConnectVnc(CConnecterDesktopThread* pConnecter,
                              QObject *parent = nullptr);
    virtual ~CConnectVnc() override;

public Q_SLOTS:
    virtual void slotConnected();
    virtual void slotDisConnected();
    virtual void slotReadyRead();
    virtual void slotClipBoardChanged() override;
    
    void slotChannelError(int nErr, const QString &szErr);

public:
    // rfb::CConnection callback methods
    virtual void initDone() override;

    // CMsgHandler interface
public:
    virtual void authSuccess() override;
    virtual void resizeFramebuffer() override;
    virtual void framebufferUpdateStart() override;
    virtual void framebufferUpdateEnd() override;
    virtual bool dataRect(const rfb::Rect &r, int encoding) override;
    virtual void setColourMapEntries(int firstColour, int nColours, uint16_t* rgbs) override;
    virtual void bell() override;
    virtual void setCursor(int width, int height, const rfb::Point& hotspot,
                              const uint8_t* data) override;
    virtual void setCursorPos(const rfb::Point &pos) override;
    virtual void handleClipboardRequest() override;
    virtual void handleClipboardAnnounce(bool available) override;
    virtual void handleClipboardData(const char *data) override;

    virtual void getUserPasswd(bool secure, std::string* user, std::string* password) override;
    virtual bool showMsgBox(int flags, const char* title, const char* text) override;

public Q_SLOTS:
    virtual void slotMousePressEvent(Qt::MouseButtons buttons, QPoint pos) override;
    virtual void slotMouseReleaseEvent(Qt::MouseButton button, QPoint pos) override;
    virtual void slotMouseMoveEvent(Qt::MouseButtons buttons, QPoint pos) override;
    virtual void slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta) override;
    virtual void slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers) override;
    virtual void slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers) override;

    
    // CConnect interface
protected:
    // Please call SetParamter before call Connect
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;
    
private:
    QSharedPointer<CChannel> m_DataChannel;
    QSharedPointer<rdr::InStream> m_InStream;
    QSharedPointer<rdr::OutStream> m_OutStream;
    
    unsigned long long m_bpsEstimate;
    unsigned m_updateCount;
    struct timeval updateStartTime;
    size_t m_updateStartPos;

    quint32 TranslateRfbKey(quint32 inkey,bool modifier);   

private:
    CParameterVnc* m_pPara;
    int SetPara();
    void autoSelectFormatAndEncoding();
    void updatePixelFormat();

    int SocketInit();
    int SetChannelConnect(QSharedPointer<CChannel> channel);
    
    int IceInit();
    int SSHInit();
};

#endif // CCONNECTVNC_H
