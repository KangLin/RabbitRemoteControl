#include "PluginTelnet.h"
#include "RabbitCommonDir.h"
#include "ConnecterTelnet.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(Telnet, "Telnet")

CPluginTelnet::CPluginTelnet(QObject *parent)
    : CPluginClient(parent)
{
}

CPluginTelnet::~CPluginTelnet()
{
    qDebug(Telnet) << "CPluginFactoryTelnet::~CPluginFactoryTelnet()";
}

const QString CPluginTelnet::Protocol() const
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
    return QIcon::fromTheme("console");
}

CConnecter *CPluginTelnet::CreateConnecter(const QString &szProtocol)
{
    if(Id() == szProtocol)
    {   
        return new CConnecterTelnet(this);
    }
    return nullptr;
}
