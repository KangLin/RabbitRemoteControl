// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "OperateFtpServer.h"
#include "PluginFtpServer.h"

static Q_LOGGING_CATEGORY(log, "Plugin.FtpServer")
CPluginFtpServer::CPluginFtpServer(QObject *parent)
    : CPlugin{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CPluginFtpServer::~CPluginFtpServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const CPlugin::TYPE CPluginFtpServer::Type() const
{
    return CPlugin::TYPE::Server;
}

const QString CPluginFtpServer::Protocol() const
{
    return "FTP";
}

const QString CPluginFtpServer::Name() const
{
    return "FtpServer";
}

const QString CPluginFtpServer::DisplayName() const
{
    return tr("FTP(File Transfer Protocol) Server");
}

const QString CPluginFtpServer::Description() const
{
    return tr("FTP(File Transfer Protocol) Server: include FTP, FTP-ES.") + "\n"
           + tr("It uses: ") + "QFtpServer: https://github.com/KangLin/QFtpServer";
}

const QString CPluginFtpServer::Version() const
{
#ifdef FtpServer_VERSION
    return FtpServer_VERSION;
#else
    return QString();
#endif
}

const QIcon CPluginFtpServer::Icon() const
{
    return QIcon::fromTheme("file-transfer");
}

const QString CPluginFtpServer::Details() const
{
    QString szDetails;
    szDetails += "- " + tr("Dependency libraries") + "\n";
    szDetails += "  - " + tr("QFtpServerLib version") + ": " + QFtpServerLib_VERSION;
    return szDetails;
}

COperate *CPluginFtpServer::OnCreateOperate(const QString &szId)
{
    if(szId != Id()) {
        qCritical(log) << "Invalid ID:" << szId;
        return nullptr;
    }
    return new COperateFtpServer(this);
}
