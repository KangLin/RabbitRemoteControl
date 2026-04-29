// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "OperateTemplateDesktop.h"
#include "PluginTemplateDesktop.h"

static Q_LOGGING_CATEGORY(log, "Plugin.TemplateDesktop")
CPluginTemplateDesktop::CPluginTemplateDesktop(QObject *parent)
    : CPlugin{parent}
{}

const CPlugin::TYPE CPluginTemplateDesktop::Type() const
{
    return CPlugin::TYPE::RemoteDesktop;
}

const QString CPluginTemplateDesktop::Protocol() const
{
    // TODO: Modify protocol
    return QString();
}

const QString CPluginTemplateDesktop::Name() const
{
    // TODO: Modify plugin name
    return "TemplateDesktop";
}

const QString CPluginTemplateDesktop::Description() const
{
    // TODO: Modify description
    return "TemplateDesktop plugin";
}

const QString CPluginTemplateDesktop::Version() const
{
#ifdef TemplateDesktop_VERSION
    return TemplateDesktop_VERSION;
#else
    return QString();
#endif
}

const QIcon CPluginTemplateDesktop::Icon() const
{
    // TODO: Modify icon
    return QIcon();
}

const QString CPluginTemplateDesktop::Details() const
{
    // TODO: add details
    return QString();
}

COperate *CPluginTemplateDesktop::OnCreateOperate(const QString &szId)
{
    if(szId != Id()) {
        qCritical(log) << "Invalid ID";
        return nullptr;
    }
    return new COperateTemplateDesktop(this);
}