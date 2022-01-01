#include "ConnecterDesktop.h"

CConnecterDesktop::CConnecterDesktop(CPluginViewer *parent)
    : CConnecterDesktopThread(parent)
{
}

int CConnecterDesktop::Connect()
{
    emit sigConnect(this);
    return 0;
}

int CConnecterDesktop::DisConnect()
{
    emit sigDisconnect(this);
    return 0;
}

