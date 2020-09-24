#ifndef CCONNECTVNC_H
#define CCONNECTVNC_H

#include <Connect.h>
#include "network/TcpSocket.h"
#include "rfb/CConnection.h"

class CConnectVnc : public CConnect,
        public rfb::CConnection, public rdr::FdInStreamBlockCallback
{
    Q_OBJECT
public:
    explicit CConnectVnc(CFrmViewer* pView = nullptr, QObject *parent = nullptr);
    virtual ~CConnectVnc();
    
    virtual int Connect();
    virtual int Exec();
    
signals:
    
public slots:

private:
    // FdInStreamBlockCallback methods
    void blockCallback();
    
    // CConnection callback methods
    rfb::CSecurity* getCSecurity(int secType);
    void serverInit();
    void setDesktopSize(int w, int h);
    void setColourMapEntries(int firstColour, int nColours, rdr::U16* rgbs);
    void bell();
    void serverCutText(const char* str, int len);
    void framebufferUpdateEnd();
    void beginRect(const rfb::Rect& r, unsigned int encoding);
    void endRect(const rfb::Rect& r, unsigned int encoding);
    void fillRect(const rfb::Rect& r, rfb::Pixel p);
    void imageRect(const rfb::Rect& r, void* p);
    void copyRect(const rfb::Rect& r, int sx, int sy);
    void setCursor(int width, int height, const rfb::Point& hotspot,
                   void* data, void* mask);
    
private:
    network::Socket* m_pSock;
};

#endif // CCONNECTVNC_H
