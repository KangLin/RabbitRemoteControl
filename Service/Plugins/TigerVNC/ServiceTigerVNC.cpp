#include "ServiceTigerVNC.h"
#include "RabbitCommonLog.h"

CServiceTigerVNC::CServiceTigerVNC(QObject *parent) : CService(parent)
{
}

bool CServiceTigerVNC::Enable()
{
    return false;
}

int CServiceTigerVNC::OnProcess()
{
    LOG_MODEL_DEBUG("ServiceTigerVNC", "Process ...");
    return 0;
}
