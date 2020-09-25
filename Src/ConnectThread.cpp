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
        connect.SetServer("127.0.0.1", 5900);
        connect.SetUser(QString(), "");
        connect.Connect();
        connect.Exec();    
    } catch (rdr::EndOfStream& e) {
        
        vlog.info(e.str());
    } catch (rdr::Exception& e) {
        vlog.error(e.str());
    }
}
