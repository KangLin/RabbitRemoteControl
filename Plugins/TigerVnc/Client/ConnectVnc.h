// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTVNC_H
#define CCONNECTVNC_H

#pragma once

#include <QEventLoop>

#include "Connect.h"
#include "Channel.h"
#include "ParameterVnc.h"

#include "ParameterConnecter.h"
#include "../InStreamChannel.h"
#include "../OutStreamChannel.h"

#include "rfb/CConnection.h"
#include "rfb/UserPasswdGetter.h"
#include "rfb/UserMsgBox.h"

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
    virtual void authSuccess() override;
    virtual void resizeFramebuffer() override;
    
    // CMsgHandler interface
public:
    virtual void setName(const char *name) override;
    virtual void framebufferUpdateStart() override;
    virtual void framebufferUpdateEnd() override;
    virtual void setColourMapEntries(int firstColour, int nColours, uint16_t* rgbs) override;
    virtual void bell() override;
    virtual void setLEDState(unsigned int state) override;
    virtual void setCursor(int width, int height, const rfb::Point& hotspot,
                              const uint8_t* data) override;
    virtual void setCursorPos(const rfb::Point &pos) override;
    virtual void fence(uint32_t flags, unsigned int len, const uint8_t data[]) override;

    virtual void handleClipboardRequest() override;
    virtual void handleClipboardAnnounce(bool available) override;
    virtual void handleClipboardData(const char *data) override;

    virtual void getUserPasswd(bool secure, std::string* user, std::string* password) override;
    virtual bool showMsgBox(int flags, const char* title, const char* text) override;

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

    // CConnect interface
protected:
    // Please call SetParamter before call Connect
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;
    virtual void WakeUp() override;

private:
    QSharedPointer<CChannel> m_DataChannel;
    QSharedPointer<rdr::InStream> m_InStream;
    QSharedPointer<rdr::OutStream> m_OutStream;
    
    quint32 TranslateRfbKey(quint32 inkey,bool modifier);
    QString ConnectInformation();

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
