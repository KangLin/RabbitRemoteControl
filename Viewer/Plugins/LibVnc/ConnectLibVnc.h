#ifndef CCONNECTLIBVNC_H
#define CCONNECTLIBVNC_H

#include "Connect.h"
#include "rfb/rfbclient.h"

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
    static void cb_text_chat(rfbClient* client, int value, char *text);
    static void cb_got_selection(rfbClient *client, const char *text, int len);
    static rfbCredential* cb_get_credential(rfbClient* cl, int credentialType);
    static char* cb_get_password(rfbClient* client);
    
public Q_SLOTS:
    virtual int Connect() override;
    virtual int Disconnect() override;
    virtual int Process() override;
    virtual void slotClipBoardChange() override;
    virtual int Initialize() override;
    virtual int Clean() override;

    virtual void slotMousePressEvent(QMouseEvent* e);
    virtual void slotMouseReleaseEvent(QMouseEvent* e);
    virtual void slotMouseMoveEvent(QMouseEvent* e);
    virtual void slotWheelEvent(QWheelEvent* e);
    virtual void slotKeyPressEvent(QKeyEvent* e);
    virtual void slotKeyReleaseEvent(QKeyEvent* e);

private:
    int OnSize();
    
private:
    rfbClient* m_pClient;
    QImage m_Image;
    
public:
    enum COLOR_LEVEL {
        Full,
        Medium,
        Low,
        VeryLow
    };
    struct strPara{
        QString szHost;
        qint16 nPort;
        QString szUser;
        QString szPassword;

        bool bSave;
        
        bool bShared;
        bool bBufferEndRefresh;
        bool bLocalCursor;
        bool bSupportsDesktopResize;
        bool bClipboard;

        bool bAutoSelect;
        COLOR_LEVEL nColorLevel;
        int nEncoding;
        bool bCompressLevel;
        int nCompressLevel;
        bool bNoJpeg;
        int nQualityLevel;
    };
private:    
    strPara* m_pPara;

protected:
    virtual int SetParamter(void *) override;
};

#endif // CCONNECTLIBVNC_H
