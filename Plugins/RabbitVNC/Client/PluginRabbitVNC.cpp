// Author: Kang Lin <kl222@126.com>

#include "PluginRabbitVNC.h"

#include "RabbitCommonDir.h"
#include "../../TigerVnc/Client/ConnecterVnc.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(log, "RabbitVNC")

CPluginRabbitVNC::CPluginRabbitVNC(QObject *parent)
    : CPluginClient(parent)
{
    //! [Initialize resource]

    //rfb::SecurityClient::setDefaults();

    //! [Initialize resource]
}

CPluginRabbitVNC::~CPluginRabbitVNC()
{
    //! [Clean resource]
    qDebug(log) << "CPluginRabbitVnc::~CPluginRabbitVnc()";
    //! [Clean resource]
}

const QString CPluginRabbitVNC::Name() const
{
    return "RabbitVNC";
}

const QString CPluginRabbitVNC::DisplayName() const
{
    return tr("RabbitVNC");
}

const QString CPluginRabbitVNC::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
           + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.") + "\n"
           + tr("The plugin is used RabbitVNC");
}

const QString CPluginRabbitVNC::Protocol() const
{
    return "RFB";
}

const QIcon CPluginRabbitVNC::Icon() const
{
    return QIcon::fromTheme("network-wired");
}

CConnecter *CPluginRabbitVNC::CreateConnecter(const QString &szID)
{
    if(Id() == szID)
    {
        return new CConnecterVnc(this);
    }
    return nullptr;
}
