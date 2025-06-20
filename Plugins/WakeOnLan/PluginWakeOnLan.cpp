// Author: Kang Lin <kl222@126.com>
#include <QLoggingCategory>
#include <QDir>

#include "PluginWakeOnLan.h"
#include "OperateWakeOnLan.h"

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
    : CPlugin{parent}
    , m_pOperate(nullptr)
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

const CPlugin::TYPE CPluginWakeOnLan::Type() const
{
    return TYPE::NetworkTools;
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

COperate* CPluginWakeOnLan::CreateOperate(const QString& szId, CParameterPlugin *para)
{
    if(!m_pOperate) {
        m_pOperate = CPlugin::CreateOperate(szId, para);
        if(!m_pOperate) return m_pOperate;

        QString szFile = m_pOperate->GetSettingsFile();
        QDir d(szFile);
        if(!d.exists(szFile)) return m_pOperate;
        int nRet = false;
        //bRet = m_pOperate->Load(szFile);
        bool bRet = QMetaObject::invokeMethod(
            m_pOperate,
            "Load",
            Qt::DirectConnection,
            Q_RETURN_ARG(int, nRet),
            Q_ARG(QString, szFile));
        if(!bRet) {
            qCritical(log) << "Call" << m_pOperate->metaObject()->className()
                         + QString("::") + "Load(QString szFile) fail." << szFile;
            return nullptr;
        }
        if(nRet) {
            qCritical(log) << "Load parameter fail" << nRet;
            DeleteOperate(m_pOperate);
            return nullptr;
        }
    }
    return m_pOperate;
}

COperate* CPluginWakeOnLan::OnCreateOperate(const QString &szId)
{
    qDebug(log) << Q_FUNC_INFO;
    if(Id() == szId)
    {
        return new COperateWakeOnLan(this);
    }
    return nullptr;
}

int CPluginWakeOnLan::DeleteOperate(COperate* p)
{
    qDebug(log) << Q_FUNC_INFO;
    Q_ASSERT(m_pOperate == p);
    m_pOperate = nullptr;
    return CPlugin::DeleteOperate(p);
}

const QString CPluginWakeOnLan::Protocol() const
{
    return QString();
}
