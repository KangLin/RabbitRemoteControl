// Author: Kang Lin <kl222@126.com>

#ifndef CConnectRabbitVnc_H
#define CConnectRabbitVnc_H

#include "Connect.h"
#include "ParameterConnecter.h"
#include "network/TcpSocket.h"
#include "rfb/CConnection.h"
#include "rfb/UserPasswdGetter.h"
#include "rfb/UserMsgBox.h"
#include "InStreamChannel.h"
#include "OutStreamChannel.h"
#include "Channel.h"
#include "ParameterRabbitVNC.h"

#include <QEventLoop>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(RabbitVNC)

class CConnecterRabbitVNC;
class CConnectRabbitVNC : public CConnect,
        public rfb::CConnection,
        public rfb::UserPasswdGetter,
        public rfb::UserMsgBox
{
    Q_OBJECT

public:
    explicit CConnectRabbitVNC(CConnecterRabbitVNC* pConnecter,
                              QObject *parent = nullptr);
    virtual ~CConnectRabbitVNC() override;

public Q_SLOTS:
    virtual void slotConnected();
    virtual void slotDisConnected();
    virtual void slotReadyRead();
    virtual void slotClipBoardChanged() override;

    void slotError(int nErr, const QString &szErr);

public:
    // rfb::CConnection callback methods
    virtual void initDone() override;

    // CMsgHandler interface
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
    QSharedPointer<rdr::InStream> m_InStream;
    QSharedPointer<rdr::OutStream> m_OutStream;
    
    unsigned long long m_bpsEstimate;
    unsigned m_updateCount;
    struct timeval updateStartTime;
    size_t m_updateStartPos;

    quint32 TranslateRfbKey(quint32 inkey,bool modifier);   

private:
    CParameterRabbitVNC* m_pPara;
    void autoSelectFormatAndEncoding();
    void updatePixelFormat();
    
    int SocketInit();
    int SetChannelConnect(QSharedPointer<CChannel> channel);
    
    int IceInit();
};

#endif // CConnectRabbitVnc_H
