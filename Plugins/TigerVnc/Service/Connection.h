#ifndef CCONNECTION_H
#define CCONNECTION_H

#include "rfb/SConnection.h"
#include <QObject>
#include "DataChannel.h"
#include "ServiceTigerVNC.h"
#include "ParameterServiceTigerVNC.h"
#include "InputDevice.h"
#include "Screen.h"

class CConnection : public QObject, rfb::SConnection
{
    Q_OBJECT
    
public:
    explicit CConnection(QTcpSocket* pSocket, CScreen* pScreen, CParameterServiceTigerVNC* pPara);
    virtual ~CConnection();

    // SConnection interface
public:
    virtual void queryConnection(const char *userName) override;
    virtual void authSuccess() override;

    // SMsgHandler interface
    virtual void clientInit(bool shared) override;
    virtual void setDesktopSize(int fb_width, int fb_height, const rfb::ScreenSet &layout) override;
    
    // InputHandler interface
    virtual void keyEvent(rdr::U32 keysym, rdr::U32 keycode, bool down) override;
    virtual void pointerEvent(const rfb::Point& pos, int buttonMask) override;
    virtual void clientCutText(const char* str) override;

private Q_SLOTS:
    void slotReadyRead();
    
private:
    CDataChannel m_DataChannel;
    CParameterServiceTigerVNC* m_pPara;
    CInputDevice m_InputDevice;
};

#endif // CCONNECTION_H
