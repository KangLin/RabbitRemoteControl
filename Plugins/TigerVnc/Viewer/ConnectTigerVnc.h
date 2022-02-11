// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTTIGERVNC_H
#define CCONNECTTIGERVNC_H

#include "Connect.h"
#include "Parameter.h"
#include "network/TcpSocket.h"
#include "rfb/CConnection.h"
#include "rfb/UserPasswdGetter.h"
#include "rfb/UserMsgBox.h"
#include "QSocketInStream.h"
#include "QSocketOutStream.h"
#include "Channel.h"
#include "ParameterTigerVnc.h"

#include <QEventLoop>

#ifdef HAVE_ICE
    #include "ICE/IceSignal.h"
#endif

class CConnecterTigerVnc;
class CConnectTigerVnc : public CConnect,
        public rfb::CConnection,
        public rfb::UserPasswdGetter,
        public rfb::UserMsgBox
{
    Q_OBJECT

public:
    explicit CConnectTigerVnc(CConnecterTigerVnc* pConnecter,
                              QObject *parent = nullptr);
    virtual ~CConnectTigerVnc() override;

public Q_SLOTS:
    virtual void slotConnected();
    virtual void slotDisConnected();
    virtual void slotReadyRead();
    virtual void slotClipBoardChange() override;

    void slotError(int nErr, const QString &szErr);

public:
    // rfb::CConnection callback methods
    virtual void initDone() override;

    // CMsgHandler interface
public:
    virtual void framebufferUpdateStart() override;
    virtual void framebufferUpdateEnd() override;
    virtual bool dataRect(const rfb::Rect &r, int encoding) override;
    virtual void setColourMapEntries(int firstColour, int nColours, rdr::U16* rgbs) override;
    virtual void bell() override;
    virtual void setCursor(int width, int height, const rfb::Point& hotspot,
                              const rdr::U8* data) override;
    virtual void setCursorPos(const rfb::Point &pos) override;
    virtual void handleClipboardRequest() override;
    virtual void handleClipboardAnnounce(bool available) override;
    virtual void handleClipboardData(unsigned int format, const char *data, size_t length) override;

    virtual void getUserPasswd(bool secure, char** user, char** password) override;
    virtual bool showMsgBox(int flags, const char *title, const char *text) override;

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
    virtual int OnInit() override;
    virtual int OnClean() override;
    
private:
    QSharedPointer<CChannel> m_DataChannel;

    unsigned long long m_bpsEstimate;
    unsigned m_updateCount;
    struct timeval updateStartTime;
    size_t m_updateStartPos;

    quint32 TranslateRfbKey(quint32 inkey,bool modifier);   

private:
    CParameterTigerVnc* m_pPara;
    void autoSelectFormatAndEncoding();
    void updatePixelFormat();
    
    int SocketInit();
    int SetChannelConnect(QSharedPointer<CChannel> channl);
    
#ifdef HAVE_ICE
    QSharedPointer<CIceSignal> m_Signal;
    int IceInit();
private Q_SLOTS:
    void slotSignalConnected();
    void slotSignalDisconnected();
    void slotSignalError(int nErr, const QString& szErr);
#endif
};

#endif // CCONNECTTIGERVNC_H
