#ifndef CCONNECTLIBVNC_H
#define CCONNECTLIBVNC_H

#include "Connect.h"
#include "rfb/rfbclient.h"
#include <QTcpSocket>
#include "ParameterLibVNCServer.h"

class CConnecterLibVNCServer;
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
    virtual void slotMousePressEvent(Qt::MouseButtons buttons, QPoint pos) override;
    virtual void slotMouseReleaseEvent(Qt::MouseButton button, QPoint pos) override;
    virtual void slotMouseMoveEvent(Qt::MouseButtons buttons, QPoint pos) override;
    virtual void slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta) override;
    virtual void slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers) override;
    virtual void slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers) override;

private:
    virtual int OnInit() override;
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
