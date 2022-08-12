// Author: Kang Lin <kl222@126.com>

#include "PluginFreeRDP.h"
#include <QDebug>
#include <QApplication>
#include "RabbitCommonDir.h"
#include "ConnecterFreeRDP.h"

CPluginFreeRDP::CPluginFreeRDP(QObject *parent)
    : CPluginClient(parent)
{
    qDebug() //<< "FreeRDP build configure:" << freerdp_get_build_config() << "\n"
             << "Version:" << freerdp_get_build_revision() << "\n"
             << "Date:" << freerdp_get_build_date();
}

CPluginFreeRDP::~CPluginFreeRDP()
{
    qDebug() << "CPluginFreeRDP::~CPluginFreeRDP()";
}

const QString CPluginFreeRDP::Name() const
{
    return "FreeRDP";
}

const QString CPluginFreeRDP::DisplayName() const
{
    return tr("Free remote desktop");
}

const QString CPluginFreeRDP::Description() const
{
    return tr("RDP(Windows remote desktop protol): Access remote desktops such as windows.");
}

const QString CPluginFreeRDP::Protol() const
{
    return "RDP";
}

const QIcon CPluginFreeRDP::Icon() const
{
    return QIcon(":/image/Windows");
}

CConnecter* CPluginFreeRDP::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        return new CConnecterFreeRDP(this);
    }
    return nullptr;
}
