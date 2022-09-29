#include "ConnecterDesktop.h"

CConnecterDesktop::CConnecterDesktop(CPluginClient *parent)
    : CConnecterDesktopThread(parent)
{
}

CConnecterDesktop::~CConnecterDesktop()
{
    qDebug(Client) << "CConnecterDesktop::~CConnecterDesktop";
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

