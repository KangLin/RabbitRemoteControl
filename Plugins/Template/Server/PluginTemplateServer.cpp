// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "OperateTemplateServer.h"
#include "PluginTemplateServer.h"

static Q_LOGGING_CATEGORY(log, "Plugin.TemplateServer")
CPluginTemplateServer::CPluginTemplateServer(QObject *parent)
    : CPlugin{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CPluginTemplateServer::~CPluginTemplateServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const CPlugin::TYPE CPluginTemplateServer::Type() const
{
    // TODO: Modify type. See: CPlugin::TYPE
    return CPlugin::TYPE::Custom;
}

const QString CPluginTemplateServer::Protocol() const
{
    // TODO: Modify protocol
    return QString();
}

const QString CPluginTemplateServer::Name() const
{
    // TODO: Modify plugin name
    return "TemplateServer";
}

const QString CPluginTemplateServer::Description() const
{
    // TODO: Modify description
    return "TemplateServer plugin";
}

const QString CPluginTemplateServer::Version() const
{
#ifdef TemplateServer_VERSION
    return TemplateServer_VERSION;
#else
    return QString();
#endif
}

const QIcon CPluginTemplateServer::Icon() const
{
    // TODO: add icon

    return QIcon();
}

const QString CPluginTemplateServer::Details() const
{
    // TODO: add details
    return QString();
}

COperate *CPluginTemplateServer::OnCreateOperate(const QString &szId)
{
    if(szId != Id()) {
        qCritical(log) << "Invalid ID";
        return nullptr;
    }
    return new COperateTemplateServer(this);
}