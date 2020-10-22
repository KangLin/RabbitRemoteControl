#ifndef CCONNECTTIGERVNC_H
#define CCONNECTTIGERVNC_H

#include "Connect.h"
#include "network/TcpSocket.h"
#include "rfb/CConnection.h"
#include "rfb/UserPasswdGetter.h"
#include "rfb/UserMsgBox.h"
#include <QObject>

class CConnectTigerVnc : public CConnect,
        public rfb::CConnection,
        public rdr::FdInStreamBlockCallback,
        public rfb::UserPasswdGetter,
        public rfb::UserMsgBox
{
    Q_OBJECT
    
public:
    explicit CConnectTigerVnc(CFrmViewer* pView = nullptr,
                              QObject *parent = nullptr);
    
    virtual int SetServerName(const QString& serverName) override;
    
    int Connect() override;
    int Exec() override;
    // FdInStreamBlockCallback methods
    void blockCallback() override;
  
    // Callback when socket is ready (or broken)
    //static void socketEvent(FL_SOCKET fd, void *data);
  
    // CConnection callback methods
    virtual void initDone() override;
    
    // CMsgHandler interface
public:
    virtual void framebufferUpdateEnd() override;
    virtual void dataRect(const rfb::Rect &r, int encoding) override;
    virtual void setColourMapEntries(int firstColour, int nColours, rdr::U16* rgbs) override;
    virtual void bell() override;    
    virtual void setCursor(int width, int height, const rfb::Point& hotspot,
                              const rdr::U8* data) override;
    
    virtual void getUserPasswd(bool secure, char** user, char** password) override;
    virtual bool showMsgBox(int flags, const char *title, const char *text) override;
    
signals:
    
public Q_SLOTS:
    virtual void slotMousePressEvent(QMouseEvent*) override;
    virtual void slotMouseReleaseEvent(QMouseEvent*) override;
    virtual void slotMouseMoveEvent(QMouseEvent*) override;
    virtual void slotWheelEvent(QWheelEvent*) override;
    virtual void slotKeyPressEvent(QKeyEvent*) override;
    virtual void slotKeyReleaseEvent(QKeyEvent*) override;
    
private:
    network::Socket* m_pSock;

};

#endif // CCONNECTTIGERVNC_H
