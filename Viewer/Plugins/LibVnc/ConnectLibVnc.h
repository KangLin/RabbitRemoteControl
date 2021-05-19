#ifndef CCONNECTLIBVNC_H
#define CCONNECTLIBVNC_H

#include "Connect.h"
#include "rfb/rfbclient.h"
#include "Parameter.h"
#include <QTcpSocket>

class CConnecterLibVnc;
class CConnectLibVnc : public CConnect
{
    Q_OBJECT

public:
    explicit CConnectLibVnc(CConnecterLibVnc* pConnecter = nullptr, QObject *parent = nullptr);
    virtual ~CConnectLibVnc() override;
    
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
    virtual int Connect() override;
    virtual int Disconnect() override;
    virtual int Process() override;
    virtual void slotClipBoardChange() override;
    virtual int Initialize() override;
    virtual int Clean() override;

    virtual void slotMousePressEvent(QMouseEvent* e) override;
    virtual void slotMouseReleaseEvent(QMouseEvent* e) override;
    virtual void slotMouseMoveEvent(QMouseEvent* e) override;
    virtual void slotWheelEvent(QWheelEvent* e) override;
    virtual void slotKeyPressEvent(QKeyEvent* e) override;
    virtual void slotKeyReleaseEvent(QKeyEvent* e) override;

private:
    int OnSize();
    bool InitClient();

private:
    rfbClient* m_pClient;
    QImage m_Image;
    
public:
    class strPara : public CParameter {
    public:
        bool bShared;
        
        bool bCompressLevel;
        int nCompressLevel;
        bool bJpeg;
        int nQualityLevel;
    };
private:    
    strPara* m_pPara;
    QTcpSocket m_tcpSocket;

protected:
    virtual int SetParamter(void *) override;
};

#endif // CCONNECTLIBVNC_H
