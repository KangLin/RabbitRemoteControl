// Author: Kang Lin <kl222@126.com>

#ifdef HAVE_LIBSSH
    #include "libssh/libssh.h"
#endif
#include <QLoggingCategory>
#include "OperateSftpServer.h"
#include "PluginSftpServer.h"

static Q_LOGGING_CATEGORY(log, "Plugin.SftpServer")
CPluginSftpServer::CPluginSftpServer(QObject *parent)
    : CPlugin{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CPluginSftpServer::~CPluginSftpServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const CPlugin::TYPE CPluginSftpServer::Type() const
{
    return CPlugin::TYPE::Service;
}

const QString CPluginSftpServer::Protocol() const
{
    return "SFTP";
}

const QString CPluginSftpServer::Name() const
{
    return "SftpServer";
}

const QString CPluginSftpServer::Description() const
{
    return "SFTP(Secure File Transfer Protocol) Server";
}

const QString CPluginSftpServer::Version() const
{
#ifdef SftpServer_VERSION
    return SftpServer_VERSION;
#else
    return QString();
#endif
}

const QIcon CPluginSftpServer::Icon() const
{
    return QIcon::fromTheme("file-transfer");
}

const QString CPluginSftpServer::Details() const
{
    QString szDetail;
#ifdef HAVE_LIBSSH
    szDetail = "  - " + tr("libssh version:") + " " + ssh_version(0) + "\n";
    ssh_session session;
    session = ssh_new();
    if(session)
    {
        szDetail += "    - " + tr("libssh protocol version:") + " " + QString::number(ssh_get_version(session)) + "\n";
        szDetail += "    - " + tr("OpenSSH server:") + " " + QString::number(ssh_get_openssh_version(session)) + "\n";

        ssh_free(session);
    }
#endif
    return szDetail;
}

COperate *CPluginSftpServer::OnCreateOperate(const QString &szId)
{
    if(szId != Id()) {
        qCritical(log) << "Invalid ID";
        return nullptr;
    }
    return new COperateSftpServer(this);
}