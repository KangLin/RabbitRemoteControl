// Author: Kang Lin <kl222@126.com>

#include "PluginFreeRdp.h"
#include <QDebug>
#include <QApplication>
#include "RabbitCommonDir.h"
#include "ConnecterFreeRdp.h"

CPluginFreeRdp::CPluginFreeRdp(QObject *parent)
    : CPluginViewer(parent)
{
}

CPluginFreeRdp::~CPluginFreeRdp()
{
    qDebug() << "CPluginFactoryFreeRdp::~CPluginFactoryFreeRdp()";
}

const QString CPluginFreeRdp::Name() const
{
    return "FreeRdp";
}

const QString CPluginFreeRdp::DisplayName() const
{
    return tr("Free Remote desktop");
}

const QString CPluginFreeRdp::Description() const
{
    return tr("RDP(Windows remote desktop protol): Access remote desktops such as windows.");
}

const QString CPluginFreeRdp::Protol() const
{
    return "RDP";
}

const QIcon CPluginFreeRdp::Icon() const
{
    return QIcon(":/image/Windows");
}

CConnecter* CPluginFreeRdp::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        return new CConnecterFreeRdp(this);
    }
    return nullptr;
}
