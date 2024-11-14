// Author: Kang Lin <kl222@126.com>

#include "PluginWakeOnLan.h"
#include "ConnecterWakeOnLan.h"
#include <QLoggingCategory>

#ifdef HAVE_PCAPPLUSPLUS
    #include "PcapPlusPlusVersion.h"
    #include "PcapDevice.h"
    #include <Logger.h>
    static Q_LOGGING_CATEGORY(logPcpp, "WOL.PCPP")
    void Pcpp_Logger(pcpp::Logger::LogLevel logLevel,
                     const std::string& logMessage,
                     const std::string& file,
                     const std::string& method,
                     const int line)
    {
        switch(logLevel)
        {
        case pcpp::Logger::LogLevel::Debug:
            qDebug(logPcpp) << logMessage.c_str();
            break;
        case pcpp::Logger::LogLevel::Info:
            qInfo(logPcpp) << logMessage.c_str();
            break;
        case pcpp::Logger::LogLevel::Error:
            qCritical(logPcpp) << logMessage.c_str();
            break;
        }
    }
#endif

static Q_LOGGING_CATEGORY(log, "WakeOnLan.Plugin")
CPluginWakeOnLan::CPluginWakeOnLan(QObject *parent)
    : CPluginClient{parent}
{
#ifdef HAVE_PCAPPLUSPLUS
    pcpp::Logger::getInstance().setLogPrinter(Pcpp_Logger);
    pcpp::Logger::getInstance().setAllModulesToLogLevel(
        pcpp::Logger::LogLevel::Debug);
    // suppressing errors to avoid cluttering stdout
    pcpp::Logger::getInstance().enableLogs();
    pcpp::Logger::getInstance().setLogLevel(
        pcpp::LogModule::PcapLogModuleLiveDevice, pcpp::Logger::Info);
#endif
}

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
    return QIcon::fromTheme("lan");
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
