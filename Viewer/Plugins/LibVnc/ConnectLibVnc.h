#ifndef CCONNECTLIBVNC_H
#define CCONNECTLIBVNC_H

#include "Connect.h"
#include "ConnecterLibVnc.h"
#include "rfb/rfbclient.h"

class CConnectLibVnc : public CConnect
{
    Q_OBJECT

public:
    explicit CConnectLibVnc(CConnecterLibVnc* pConnecter = nullptr, QObject *parent = nullptr);
    virtual ~CConnectLibVnc() override;
    
    static rfbBool cb_resize(rfbClient* client);
    static void cb_update (rfbClient *cl, int x, int y, int w, int h);
    static void cb_kbd_leds(rfbClient* cl, int value, int pad);
    static void cb_text_chat(rfbClient* cl, int value, char *text);
    static void cb_got_selection(rfbClient *cl, const char *text, int len);
    static rfbCredential* cb_get_credential(rfbClient* cl, int credentialType);
    
public slots:
    virtual int Connect() override;
    virtual int Disconnect() override;
    virtual int Process() override;
    virtual void slotClipBoardChange() override;
    virtual int Initialize() override;
    virtual int Clean() override;

private:
    rfbClient* m_pClient;
};

#endif // CCONNECTLIBVNC_H
