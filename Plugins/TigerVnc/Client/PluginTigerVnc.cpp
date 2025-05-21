// Author: Kang Lin <kl222@126.com>

#include "PluginTigerVnc.h"
#include "ConnecterVnc.h"
#include "RabbitCommonDir.h"

#ifdef HAVE_LIBSSH
    #include "ChannelSSHTunnel.h"
#endif

#include <QLoggingCategory>
static Q_LOGGING_CATEGORY(log, "VNC.Plugin.Tiger")

CPluginTigerVnc::CPluginTigerVnc(QObject *parent)
    : CPluginClient(parent)
{
    qDebug(log) << "CPluginTigerVnc::CPluginTigerVnc()";
}

CPluginTigerVnc::~CPluginTigerVnc()
{
    qDebug(log) << "CPluginTigerVnc::~CPluginTigerVnc()";
}

const QString CPluginTigerVnc::Name() const
{
    return "TigerVNC";
}

const QString CPluginTigerVnc::DisplayName() const
{
    return tr("TigerVNC");
}

const QString CPluginTigerVnc::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
           + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.") + "\n"
           + tr("The plugin is used TigerVNC");
}

const QString CPluginTigerVnc::Protocol() const
{
    return "RFB";
}

const QIcon CPluginTigerVnc::Icon() const
{
    return QIcon::fromTheme("vnc");
}

const QString CPluginTigerVnc::Version() const
{
    return PluginTigerVNC_VERSION;
}

const QString CPluginTigerVnc::Details() const
{
    QString szDetails;
#ifdef HAVE_LIBSSH
    CChannelSSHTunnel channel(nullptr, nullptr, nullptr);
    szDetails += channel.GetDetails();
#endif
    return szDetails;
}

CConnecter *CPluginTigerVnc::OnCreateConnecter(const QString &szID)
{
    if(Id() == szID)
    {
        return new CConnecterVnc(this);
    }
    return nullptr;
}
