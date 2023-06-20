// Author: Kang Lin <kl222@126.com>

#include "PluginFreeRDP.h"
#include "RabbitCommonDir.h"
#include "ConnecterFreeRDP.h"
#include "winpr/wlog.h"

#include <QDebug>
#include <QApplication>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(LoggerPlugin, "FreeRDP.Plugin")

CPluginFreeRDP::CPluginFreeRDP(QObject *parent)
    : CPluginClient(parent)
{
    qDebug(LoggerPlugin) << Details();
    //WLog_SetLogLevel(WLog_GetRoot(), WLOG_TRACE);
}

CPluginFreeRDP::~CPluginFreeRDP()
{
    qDebug(LoggerPlugin) << "CPluginFreeRDP::~CPluginFreeRDP()";
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
    return tr("RDP(Windows remote desktop Protocol): Access remote desktops such as windows.");
}

const QString CPluginFreeRDP::Protocol() const
{
    return "RDP";
}

const QIcon CPluginFreeRDP::Icon() const
{
    return QIcon::fromTheme("windows");
}

const QString CPluginFreeRDP::Details() const
{
    QString szDetails;
    szDetails = tr("- FreeRDP version:");
    szDetails += freerdp_get_version_string();
    szDetails += "\n";
    szDetails += tr("- Build version:");
    szDetails += freerdp_get_build_revision();
    szDetails += ":";
    szDetails += freerdp_get_build_revision();
    szDetails += "\n";
    szDetails += tr("- Build date:");
    szDetails += freerdp_get_build_date();
    szDetails += "\n";
    szDetails += "- ";
    szDetails += freerdp_get_build_config();
    return szDetails;
}

CConnecter* CPluginFreeRDP::CreateConnecter(const QString &szProtocol)
{
    if(Id() == szProtocol)
    {   
        return new CConnecterFreeRDP(this);
    }
    return nullptr;
}
