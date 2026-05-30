// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "OperateTemplateBase.h"
#include "PluginTemplateBase.h"

static Q_LOGGING_CATEGORY(log, "Plugin.TemplateBase")
CPluginTemplateBase::CPluginTemplateBase(QObject *parent)
    : CPlugin{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CPluginTemplateBase::~CPluginTemplateBase()
{
    qDebug(log) << Q_FUNC_INFO;
}

const CPlugin::TYPE CPluginTemplateBase::Type() const
{
    // TODO: Modify type. See: CPlugin::TYPE
    return CPlugin::TYPE::Custom;
}

const QString CPluginTemplateBase::Protocol() const
{
    // TODO: Modify protocol
    return QString();
}

const QString CPluginTemplateBase::Name() const
{
    // TODO: Modify plugin name
    return "TemplateBase";
}

const QString CPluginTemplateBase::Description() const
{
    // TODO: Modify description
    return "TemplateBase plugin";
}

const QString CPluginTemplateBase::Version() const
{
#ifdef TemplateBase_VERSION
    return TemplateBase_VERSION;
#else
    return QString();
#endif
}

const QIcon CPluginTemplateBase::Icon() const
{
    // TODO: add icon
    return QIcon();
}

const QString CPluginTemplateBase::Details() const
{
    // TODO: add details
    return QString();
}

COperate *CPluginTemplateBase::OnCreateOperate(const QString &szId)
{
    if(szId != Id()) {
        qCritical(log) << "Invalid ID:" << szId;
        return nullptr;
    }
    return new COperateTemplateBase(this);
}
