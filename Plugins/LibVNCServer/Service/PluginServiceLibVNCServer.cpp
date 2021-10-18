#include "PluginServiceLibVNCServer.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include <QDebug>
#include "ServiceLibVNCServer.h"

CPluginServiceLibVNCServer::CPluginServiceLibVNCServer(QObject *parent)
    : CPluginService(parent)
{
}

CPluginServiceLibVNCServer::~CPluginServiceLibVNCServer()
{
    qDebug() << "CPluginServiceLibVNCServer::~CPluginServiceLibVNCServer()";
}

const QString CPluginServiceLibVNCServer::Protol() const
{
    return "RFB";
}

const QString CPluginServiceLibVNCServer::Name() const
{
    return tr("LibVNCServer");
}

const QString CPluginServiceLibVNCServer::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

CService *CPluginServiceLibVNCServer::NewService()
{
    return new CServiceLibVNCServer(this);
}
