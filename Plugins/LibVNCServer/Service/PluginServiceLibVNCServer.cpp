#include "PluginServiceLibVNCServer.h"
#include "RabbitCommonDir.h"
#include "ServiceLibVNCServer.h"

#include <QLoggingCategory>
Q_LOGGING_CATEGORY(LibVNCServer, "LibVNCServer")

CPluginServiceLibVNCServer::CPluginServiceLibVNCServer(QObject *parent)
    : CPluginServiceThread(parent)
{
}

CPluginServiceLibVNCServer::~CPluginServiceLibVNCServer()
{
    qDebug(LibVNCServer) << "CPluginServiceLibVNCServer::~CPluginServiceLibVNCServer()";
}

const QString CPluginServiceLibVNCServer::Protocol() const
{
    return "RFB";
}

const QString CPluginServiceLibVNCServer::Name() const
{
    return "LibVNCServer";
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

