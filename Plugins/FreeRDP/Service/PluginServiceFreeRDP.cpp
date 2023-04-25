// Author: Kang Lin <kl222@126.com>

#include "PluginServiceFreeRDP.h"
#include "ServiceFreeRDP.h"

CPluginServiceFreeRDP::CPluginServiceFreeRDP(QObject *parent)
    : CPluginService{parent}
{}

CPluginServiceFreeRDP::~CPluginServiceFreeRDP()
{}

const QString CPluginServiceFreeRDP::Protocol() const
{
    return "RDP";
}

const QString CPluginServiceFreeRDP::Name() const
{
    return "FreeRDP";
}

const QString CPluginServiceFreeRDP::DisplayName() const
{
    return tr("FreeRDP");
}

const QString CPluginServiceFreeRDP::Description() const
{
    return tr("RDP(Windows remote desktop protocol): Access remote desktops such as windows.");
}

CService *CPluginServiceFreeRDP::NewService()
{
    return new CServiceFreeRDP(this);
}
