#ifndef CCONNECTVNC_H
#define CCONNECTVNC_H

#pragma once

#include <Connect.h>
#include "network/TcpSocket.h"
#include "rfb/CConnection.h"
#include "rfb/UserPasswdGetter.h"

class CConnectVnc : public CConnect,
        public rfb::CConnection, public rdr::FdInStreamBlockCallback,
        public rfb::UserPasswdGetter
{
    Q_OBJECT
public:
    explicit CConnectVnc(CFrmViewer* pView = nullptr, QObject *parent = nullptr);
    virtual ~CConnectVnc() override;
    
    virtual int Connect() override;
    virtual int Exec() override;
        
signals:
    
public slots:

private:
    // FdInStreamBlockCallback methods
    void blockCallback() override;
    // UserPasswdGetter methods
    void getUserPasswd(char** user, char** password) override;
    
    // CConnection callback methods
    rfb::CSecurity* getCSecurity(int secType) override;
    
    void serverInit() override;
    void setDesktopSize(int w, int h) override;
    void setColourMapEntries(int firstColour, int nColours, rdr::U16* rgbs) override;
    void bell() override;
    void serverCutText(const char* str, int len) override;
    
    void framebufferUpdateEnd() override;
    void beginRect(const rfb::Rect& r, unsigned int encoding) override;
    void endRect(const rfb::Rect& r, unsigned int encoding) override;
    void fillRect(const rfb::Rect& r, rfb::Pixel p) override;
    void imageRect(const rfb::Rect& r, void* p) override;
    void copyRect(const rfb::Rect& r, int sx, int sy) override;
    void setCursor(int width, int height, const rfb::Point& hotspot,
                   void* data, void* mask) override;
    
private:
    network::Socket* m_pSock;
};

#endif // CCONNECTVNC_H
