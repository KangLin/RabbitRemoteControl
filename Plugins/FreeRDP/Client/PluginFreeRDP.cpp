// Author: Kang Lin <kl222@126.com>

#include "PluginFreeRDP.h"
#include "ConnecterFreeRDP.h"
#include "winpr/wlog.h"

#include <QLoggingCategory>

#ifdef HAVE_LIBSSH
    #include "ChannelSSHTunnel.h"
#endif

static Q_LOGGING_CATEGORY(log, "FreeRDP.Plugin")
static Q_LOGGING_CATEGORY(LoggerFreeRDP, "FreeRDP.Log")

//! [Initialize resource]
CPluginFreeRDP::CPluginFreeRDP(QObject *parent)
    : CPluginClient(parent)
{
    qDebug(log) << Q_FUNC_INFO;
    qInfo(log) << "FreeRDP version:" << freerdp_get_version_string()
                        << "revision:" << freerdp_get_build_revision();

    static wLogCallbacks* pCbLog = new wLogCallbacks;
    BOOL bRet = WLog_SetLogAppenderType(WLog_GetRoot(), WLOG_APPENDER_CALLBACK);
    if(bRet && pCbLog)
    {
        memset(pCbLog, 0, sizeof(wLogCallbacks));
        pCbLog->message = [](const wLogMessage* msg)->BOOL{
            switch(msg->Level)
            {
            case WLOG_TRACE:
                break;
            case WLOG_DEBUG:
                qDebug(LoggerFreeRDP) /*<< msg->PrefixString */ << msg->TextString;
                break;
            case WLOG_INFO:
                qInfo(LoggerFreeRDP) /*<< msg->PrefixString*/ << msg->TextString;
                break;
            case WLOG_WARN:
                qWarning(LoggerFreeRDP) /*<< msg->PrefixString */<< msg->TextString;
            case WLOG_ERROR:
                qCritical(LoggerFreeRDP) /*<< msg->PrefixString*/ << msg->TextString;
                break;
            case WLOG_FATAL:
            case WLOG_OFF:
                qFatal(msg->FormatString, msg->TextString);
                break;
            default:
                break;
            }
            return TRUE;
        };
        WLog_ConfigureAppender(WLog_GetLogAppender(WLog_GetRoot()), "callbacks", pCbLog);
    }
    WLog_SetLogLevel(WLog_GetRoot(), WLOG_TRACE);
}
//! [Initialize resource]

//! [Clean resource]
CPluginFreeRDP::~CPluginFreeRDP()
{
    qDebug(log) << Q_FUNC_INFO;
}
//! [Clean resource]

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

const QString CPluginFreeRDP::Version() const
{
    return PluginFreeDP_VERSION;
}

/*!
 * \brief Show the plugin depends on the freerdp version 
 */
const QString CPluginFreeRDP::Details() const
{
    QString szDetails;
    szDetails = "- " + tr("FreeRDP");
    szDetails += "\n";
    szDetails += "  - " + tr("version: ");
    szDetails += freerdp_get_version_string();
    szDetails += "\n";
    szDetails += "  - " + tr("Build version: ");
    szDetails += freerdp_get_build_revision();
    szDetails += ":";
    szDetails += freerdp_get_build_revision();
    szDetails += "\n";
#if FreeRDP_VERSION_MAJOR < 3
    szDetails += "  - " + tr("Build date: ");
    szDetails += freerdp_get_build_date();
    szDetails += "\n";
#endif
    szDetails += "  - ";
    szDetails += freerdp_get_build_config();

#ifdef HAVE_LIBSSH
    QSharedPointer<CParameterChannelSSH> parameter(new CParameterChannelSSH());
    CChannelSSHTunnel channel(parameter);
    szDetails += channel.GetDetails();
#endif

    return szDetails;
}

CConnecter* CPluginFreeRDP::CreateConnecter(const QString &szId)
{
    if(Id() == szId)
    {   
        return new CConnecterFreeRDP(this);
    }
    return nullptr;
}
