#include "ConnectThread.h"
#include "ConnectTigerVnc.h"
#include "rfb/LogWriter.h"
#include "rfb/Security.h"
#ifdef HAVE_GNUTLS
#include "rfb/CSecurityTLS.h"
#endif
static rfb::LogWriter vlog("ConnectThread");

CConnectThread::CConnectThread(CFrmViewer *pView, QObject *parent)
    : QThread(parent),
      m_pView(pView)
{}

void CConnectThread::run()
{
    try {
        CConnectTigerVnc c(m_pView);
        //c.SetServer("fmpixel.f3322.net", 5906);
        //c.SetUser(QString(), "yly075077");
        c.SetServer("127.0.0.1", 5900);
        c.SetUser(QString(), "123456");
        rfb::CSecurity::upg = &c;
#ifdef HAVE_GNUTLS
        rfb::CSecurityTLS::msg = &c;
#endif
        c.Connect();
        c.Exec();
    } catch (rdr::Exception& e) {
        vlog.error(e.str());
    }  catch (...) { 
        vlog.error("ConnectThread error");
    }
}
