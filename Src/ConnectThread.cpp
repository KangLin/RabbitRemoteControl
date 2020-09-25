#include "ConnectThread.h"
#include "ConnectVnc.h"
#include "rfb/LogWriter.h"

static rfb::LogWriter vlog("ConnectThread");

CConnectThread::CConnectThread(CFrmViewer *pView, QObject *parent)
    : QThread(parent),
      m_pView(pView)
{}

void CConnectThread::run()
{
    try {
        CConnectVnc connect(m_pView);
        connect.SetServer("fmpixel.f3322.net", 5902);
        connect.SetUser(QString(), "yly075077");
        connect.Connect();
        connect.Exec();    
    } catch (rdr::EndOfStream& e) {
        
        vlog.info(e.str());
    } catch (rdr::Exception& e) {
        vlog.error(e.str());
    }
}
