// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "OperateTemplateQtEvent.h"
#include "PluginTemplateQtEvent.h"

static Q_LOGGING_CATEGORY(log, "Plugin.TemplateQtEvent")
CPluginTemplateQtEvent::CPluginTemplateQtEvent(QObject *parent)
    : CPluginBackendThread{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CPluginTemplateQtEvent::~CPluginTemplateQtEvent()
{
    qDebug(log) << Q_FUNC_INFO;
}

const CPlugin::TYPE CPluginTemplateQtEvent::Type() const
{
    // TODO: Modify type. See: CPlugin::TYPE
    return CPlugin::TYPE::Custom;
}

const QString CPluginTemplateQtEvent::Protocol() const
{
    // TODO: Modify protocol
    return QString();
}

const QString CPluginTemplateQtEvent::Name() const
{
    // TODO: Modify plugin name
    return "TemplateQtEvent";
}

const QString CPluginTemplateQtEvent::Description() const
{
    // TODO: Modify description
    return "TemplateQtEvent plugin";
}

const QString CPluginTemplateQtEvent::Version() const
{
#ifdef TemplateQtEvent_VERSION
    return TemplateQtEvent_VERSION;
#else
    return QString();
#endif
}

const QIcon CPluginTemplateQtEvent::Icon() const
{
    // TODO: add icon
    return QIcon();
}

const QString CPluginTemplateQtEvent::Details() const
{
    // TODO: add details
    return QString();
}

COperate *CPluginTemplateQtEvent::OnCreateOperate(const QString &szId)
{
    if(szId != Id()) {
        qCritical(log) << "Invalid ID:" << szId;
        return nullptr;
    }
    return new COperateTemplateQtEvent(this);
}
