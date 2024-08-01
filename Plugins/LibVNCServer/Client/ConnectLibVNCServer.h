#ifndef CCONNECTLIBVNC_H
#define CCONNECTLIBVNC_H

#pragma once

#include "Connect.h"
#include "rfb/rfbclient.h"
#include <QTcpSocket>
#include "ConnecterLibVNCServer.h"

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
    virtual void slotMousePressEvent(QMouseEvent* event, QPoint pos) override;
    virtual void slotMouseReleaseEvent(QMouseEvent* event, QPoint pos) override;
    virtual void slotMouseMoveEvent(QMouseEvent* event, QPoint pos) override;
    virtual void slotWheelEvent(QWheelEvent* event, QPoint pos) override;
    virtual void slotKeyPressEvent(QKeyEvent *event) override;
    virtual void slotKeyReleaseEvent(QKeyEvent *event) override;

private:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;

    int OnSize();
    bool InitClient();

private:
    rfbClient* m_pClient;
    QImage m_Image;

private:    
    CParameterLibVNCServer* m_pPara;
    QTcpSocket m_tcpSocket;
};

#endif // CCONNECTLIBVNC_H
