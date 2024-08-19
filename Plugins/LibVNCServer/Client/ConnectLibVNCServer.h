#ifndef CCONNECTLIBVNC_H
#define CCONNECTLIBVNC_H

#pragma once

#include "Connect.h"
#include "rfb/rfbclient.h"
#include <QTcpSocket>
#include <QTcpServer>
#include "ConnecterLibVNCServer.h"
#include "Event.h"

#ifdef HAVE_LIBSSH
#include "SSHTunnelThread.h"
#endif

class CConnectLibVNCServer : public CConnect
{
    Q_OBJECT

public:
    explicit CConnectLibVNCServer(CConnecterLibVNCServer* pConnecter = nullptr, QObject *parent = nullptr);
    virtual ~CConnectLibVNCServer() override;
    
    static rfbBool cb_resize(rfbClient* client);
    static void cb_update (rfbClient *client, int x, int y, int w, int h);
    static void cb_kbd_leds(rfbClient* client, int value, int pad);
    static void cb_bell(struct _rfbClient* client);
    static void cb_text_chat(rfbClient* client, int value, char *text);
    static void cb_got_selection(rfbClient *client, const char *text, int len);
    static rfbCredential* cb_get_credential(rfbClient* cl, int credentialType);
    static char* cb_get_password(rfbClient* client);
    static rfbBool cb_cursor_pos(rfbClient* client, int x, int y);
    static void cb_got_cursor_shape(rfbClient* client, int xhot, int yhot, int width, int height, int bytesPerPixel);
    
public Q_SLOTS:
    virtual void slotClipBoardChanged() override;
    virtual void mousePressEvent(QMouseEvent* event) override;
    virtual void mouseReleaseEvent(QMouseEvent* event) override;
    virtual void mouseMoveEvent(QMouseEvent* event) override;
    virtual void wheelEvent(QWheelEvent* event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    
    void slotConnectProxyServer(quint16 nPort);
#if defined(HAVE_UNIX_DOMAIN_SOCKET) && defined(Q_OS_UNIX)
    void slotConnectProxyServer(QString szFile);
#endif

private:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;
    virtual int WakeUp() override;

    int OnSize();

private:
    rfbClient* m_pClient;
    QImage m_Image;

private:    
    CParameterLibVNCServer* m_pParameter;
    QTcpSocket m_tcpSocket;
    QTcpServer m_Server;
    QTcpSocket* m_pConnect;
    
    Channel::CEvent m_Event;
#ifdef HAVE_LIBSSH
    CSSHTunnelThread* m_pThread;
#endif
};

#endif // CCONNECTLIBVNC_H
