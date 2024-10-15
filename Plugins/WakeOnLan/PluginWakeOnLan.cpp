// Author: Kang Lin <kl222@126.com>

#include "PluginWakeOnLan.h"
#include "ConnecterWakeOnLan.h"
#include <QLoggingCategory>

#ifdef HAVE_PCAPPLUSPLUS
    #include "PcapPlusPlusVersion.h"
    #include "PcapDevice.h"
#endif

static Q_LOGGING_CATEGORY(log, "WakeOnLan.Plugin")
CPluginWakeOnLan::CPluginWakeOnLan(QObject *parent)
    : CPluginClient{parent}
{}

CPluginWakeOnLan::~CPluginWakeOnLan()
{
    qDebug(log) << "CPluginWakeOnLan::~CPluginWakeOnLan()";
}

const QString CPluginWakeOnLan::Protocol() const
{
    return "Tool";
}

const QString CPluginWakeOnLan::Name() const
{
    return "WakeOnLan";
}

const QString CPluginWakeOnLan::DisplayName() const
{
    return tr("Wake on lan");
}

const QString CPluginWakeOnLan::Description() const
{
    return tr("Wake on lan");
}

const QIcon CPluginWakeOnLan::Icon() const
{
    return QIcon::fromTheme("tools");
}

const QString CPluginWakeOnLan::Version() const
{
    return PluginWakeOnLan_VERSION;
}

const QString CPluginWakeOnLan::Details() const
{
    QString szDetails;
#ifdef HAVE_PCAPPLUSPLUS
    szDetails = "- PcapPlusPlus";
    szDetails += "\n";
    szDetails += "  - " + tr("version: ")
                 + pcpp::getPcapPlusPlusVersionFull().c_str();
    szDetails += "\n";
    szDetails += "    - " + tr("Pcap lib version info: ")
                 + pcpp::IPcapDevice::getPcapLibVersionInfo().c_str();
#endif
    return szDetails;
}

CConnecter *CPluginWakeOnLan::CreateConnecter(const QString &szId)
{
    if(Id() == szId)
    {
        return new CConnecterWakeOnLan(this);
    }
    return nullptr;
}
