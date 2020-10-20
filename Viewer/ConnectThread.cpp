#include "ConnectThread.h"
#include "ConnectTigerVnc.h"
#include "rfb/LogWriter.h"
#include "rfb/Security.h"
#include "rfb/CSecurityTLS.h"

static rfb::LogWriter vlog("ConnectThread");

CConnectThread::CConnectThread(CFrmViewer *pView, QObject *parent)
    : QThread(parent),
      m_pView(pView)
{}

void CConnectThread::run()
{
    try {
        CConnectTigerVnc c(m_pView);
        c.SetServer("fmpixel.f3322.net", 5906);
        c.SetUser(QString(), "yly075077");
        rfb::CSecurity::upg = &c;
        rfb::CSecurityTLS::msg = &c;
        c.Connect();
        c.Exec();
    } catch (rdr::Exception& e) {
        vlog.error(e.str());
    }  catch (...) { 
        vlog.error("ConnectThread error");
    }
}
