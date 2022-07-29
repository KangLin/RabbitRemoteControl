#include "PluginTelnet.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include "ConnecterTelnet.h"

#include <QDebug>

CPluginTelnet::CPluginTelnet(QObject *parent)
    : CPluginClient(parent)
{
}

CPluginTelnet::~CPluginTelnet()
{
    qDebug() << "CPluginFactoryTelnet::~CPluginFactoryTelnet()";
}

const QString CPluginTelnet::Protol() const
{
    return "Telnet";
}

const QString CPluginTelnet::Name() const
{
    return tr("Telnet");
}

const QString CPluginTelnet::Description() const
{
    return tr("Telnet: net terminal");
}

const QIcon CPluginTelnet::Icon() const
{
    return QIcon(":/image/Console");
}

CConnecter *CPluginTelnet::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        return new CConnecterTelnet(this);
    }
    return nullptr;
}
