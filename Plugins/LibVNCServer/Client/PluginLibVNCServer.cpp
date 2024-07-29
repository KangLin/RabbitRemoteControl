// Author: Kang Lin <kl222@126.com>

#include "PluginLibVNCServer.h"
#include "RabbitCommonDir.h"
#include "ConnecterLibVNCServer.h"

#include <QLoggingCategory>
static Q_LOGGING_CATEGORY(log, "LibVNCServer")

CPluginLibVNCServer::CPluginLibVNCServer(QObject *parent)
    : CPluginClient(parent)
{}

CPluginLibVNCServer::~CPluginLibVNCServer()
{
    qDebug(log) << "CPluginFactoryLibVNCServer::~CPluginFactoryLibVNCServer()";
}

const QString CPluginLibVNCServer::Name() const
{
    return "LibVNCServer";
}

const QString CPluginLibVNCServer::DisplayName() const
{
    return tr("LibVNCServer");
}

const QString CPluginLibVNCServer::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.") + "\n"
           + tr("The plugin is used LibVNCServer");
}

const QString CPluginLibVNCServer::Protocol() const
{
    return "RFB";
}

const QIcon CPluginLibVNCServer::Icon() const
{
    return QIcon::fromTheme("network-wired");
}

CConnecter *CPluginLibVNCServer::CreateConnecter(const QString &szID)
{
    if(Id() == szID)
    {   
        return new CConnecterLibVNCServer(this);
    }
    return nullptr;
}
