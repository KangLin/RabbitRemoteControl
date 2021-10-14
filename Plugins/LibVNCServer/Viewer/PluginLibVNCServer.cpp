#include "PluginLibVNCServer.h"
#include "RabbitCommonDir.h"
#include "ConnecterLibVNCServer.h"

#include <QDebug>

CPluginLibVNCServer::CPluginLibVNCServer(QObject *parent) : CPluginViewer(parent)
{
}

CPluginLibVNCServer::~CPluginLibVNCServer()
{
    qDebug() << "CPluginFactoryLibVNCServer::~CPluginFactoryLibVNCServer()";
}

const QString CPluginLibVNCServer::Name() const
{
    return tr("LibVNCServer");
}

const QString CPluginLibVNCServer::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

const QString CPluginLibVNCServer::Protol() const
{
    return "RFB";
}

const QIcon CPluginLibVNCServer::Icon() const
{
    return QIcon(":/image/Connect");
}

CConnecter *CPluginLibVNCServer::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        return new CConnecterLibVNCServer(this);
    }
    return nullptr;
}
