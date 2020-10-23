#include "ConnectThread.h"
#include "ConnectTigerVnc.h"

CConnectThread::CConnectThread(CFrmViewer *pView, QObject *parent) : QThread(parent)
{
    m_pView = pView;
}

void CConnectThread::run()
{
    CConnectTigerVnc c(m_pView);
    c.Connect();
    c.Exec();
}
