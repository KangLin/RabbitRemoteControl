#include "ConnecterDesktop.h"
#include "RabbitCommonLog.h"

CConnecterDesktop::CConnecterDesktop(CPluginClient *parent)
    : CConnecterDesktopThread(parent)
{
}

CConnecterDesktop::~CConnecterDesktop()
{
    LOG_MODEL_DEBUG("CConnecterDesktop", "CConnecterDesktop::~CConnecterDesktop");
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

