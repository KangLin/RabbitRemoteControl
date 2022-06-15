// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTION_H
#define CCONNECTION_H

#include "rfb/SConnection.h"
#include "rfb/EncodeManager.h"
#include "rfb/UpdateTracker.h"

#include <QImage>
#include <QRect>

#include "Channel.h"
#include "InStreamChannel.h"
#include "OutStreamChannel.h"
#include "ServiceRabbitVNC.h"
#include "ParameterServiceRabbitVNC.h"
#include "InputDevice.h"
#include "Screen.h"

class CConnection : public QObject, public rfb::SConnection
{
    Q_OBJECT
    
public:
    explicit CConnection(QSharedPointer<CChannel> channel, CParameterServiceRabbitVNC* pPara);
    virtual ~CConnection();

    // SConnection interface
public:
    virtual void queryConnection(const char *userName) override;
    virtual void authSuccess() override;

    // SMsgHandler interface
    virtual void clientInit(bool shared) override;
    virtual void setDesktopSize(int fb_width, int fb_height, const rfb::ScreenSet &layout) override;   
    virtual void setPixelFormat(const rfb::PixelFormat &pf) override;
    virtual void framebufferUpdateRequest(const rfb::Rect &r, bool incremental) override;    
    virtual void fence(rdr::U32 flags, unsigned len, const char data[]) override;
    virtual void enableContinuousUpdates(bool enable, int x, int y, int w, int h) override;
    
    // InputHandler interface
    virtual void keyEvent(rdr::U32 keysym, rdr::U32 keycode, bool down) override;
    virtual void pointerEvent(const rfb::Point& pos, int buttonMask) override;
    virtual void clientCutText(const char* str) override;

Q_SIGNALS:
    void sigDisconnected();
    void sigError(int nErr, const QString& szErr);

private:
    QSharedPointer<rfb::PixelBuffer> GetBufferFromQImage(QImage &img);
    void writeNoDataUpdate();
    void writeDataUpdate(QImage img, QRect rect);
    
private Q_SLOTS:
    void slotConnected();
    void slotReadyRead();
    void slotDisconnected();
    void slotError(int nRet, const QString &szErr);
    void slotDesktopUpdate(QImage img, QRect rect);

private:
    QSharedPointer<CChannel> m_Channel;
    QSharedPointer<rdr::InStream> m_InStream;
    QSharedPointer<rdr::OutStream> m_OutStream;
    CParameterServiceRabbitVNC* m_pPara;
    QSharedPointer<CInputDevice> m_InputDevice;
    
    rfb::PixelFormat m_PixelFormat;

    bool inProcessMessages;
    
    bool pendingSyncFence, syncFence;
    rdr::U32 fenceFlags;
    unsigned fenceDataLen;
    char *fenceData;

    rfb::EncodeManager m_EncodeManager;
    rfb::SimpleUpdateTracker m_Updates;
    rfb::Region requested;
    bool continuousUpdates;
    rfb::Region cuRegion;

};

#endif // CCONNECTION_H
