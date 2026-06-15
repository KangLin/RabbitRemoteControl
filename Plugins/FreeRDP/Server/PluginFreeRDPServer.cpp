// Author: Kang Lin <kl222@126.com>

#include <freerdp/server/shadow.h>
#include <freerdp/version.h>
#include "winpr/wlog.h"

#include <QLoggingCategory>

#ifdef HAVE_LIBSSH
#include "ChannelSSHTunnel.h"
#endif

#include "OperateFreeRDPServer.h"
#include "PluginFreeRDPServer.h"

static Q_LOGGING_CATEGORY(log, "FreeRDPServer.Plugin")
static Q_LOGGING_CATEGORY(LoggerFreeRDP, "FreeRDP.Log")
static Q_LOGGING_CATEGORY(LoggerFreeRDPTrace, "FreeRDP.Log.Trace")
static Q_LOGGING_CATEGORY(LoggerFreeRDPFatal, "FreeRDP.Log.Fatal")
CPluginFreeRDPServer::CPluginFreeRDPServer(QObject *parent)
    : CPlugin{parent}
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
                qDebug(LoggerFreeRDPTrace) /*<< msg->PrefixString */ << msg->TextString;
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
                qCritical(LoggerFreeRDPFatal) /*<< msg->PrefixString*/ << msg->TextString;
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

CPluginFreeRDPServer::~CPluginFreeRDPServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const CPlugin::TYPE CPluginFreeRDPServer::Type() const
{
    return TYPE::Server;
}

const QString CPluginFreeRDPServer::Protocol() const
{
    return "RDP";
}

const QString CPluginFreeRDPServer::Name() const
{
    return "FreeRDPServer";
}

const QString CPluginFreeRDPServer::Description() const
{
    return "FreeRDP - Server";
}

const QString CPluginFreeRDPServer::Version() const
{
#ifdef FreeRDPServer_VERSION
    return FreeRDPServer_VERSION;
#else
    return QString();
#endif
}

const QIcon CPluginFreeRDPServer::Icon() const
{
    return QIcon::fromTheme("windows");
}

const QString CPluginFreeRDPServer::Details() const
{
    QString szDetails;
    szDetails += "- " + tr("Dependency libraries") + "\n";
    szDetails += "  - " + tr("FreeRDP") + "\n";
    szDetails += "    - " + tr("version: ");
    szDetails += freerdp_get_version_string();
    szDetails += "\n";
    szDetails += "    - " + tr("Build version: ");
    szDetails += freerdp_get_build_revision();
    szDetails += ":";
    szDetails += freerdp_get_build_revision();
    szDetails += "\n";
#if FREERDP_VERSION_MAJOR < 3
    szDetails += "    - " + tr("Build date: ");
    szDetails += freerdp_get_build_date();
    szDetails += "\n";
#endif
    szDetails += "    - ";
    szDetails += freerdp_get_build_config();
    szDetails += "\n";

#ifdef HAVE_LIBSSH
    CChannelSSH channel(nullptr, nullptr);
    szDetails += channel.GetDetails();
#endif

    return szDetails;
}

COperate *CPluginFreeRDPServer::OnCreateOperate(const QString &szId)
{
    if(szId != Id()) {
        qCritical(log) << "Invalid ID:" << szId;
        return nullptr;
    }
    return new COperateFreeRDPServer(this);
}
