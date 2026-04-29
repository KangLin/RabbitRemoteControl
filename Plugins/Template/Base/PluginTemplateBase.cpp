// Author: Kang Lin <kl222@126.com>

#include "OperateTemplateBase.h"
#include "PluginTemplateBase.h"

CPluginTemplateBase::CPluginTemplateBase(QObject *parent)
    : CPlugin{parent}
{}

const CPlugin::TYPE CPluginTemplateBase::Type() const
{
    // TODO: add type. See: CPlugin::TYPE
    return CPlugin::TYPE::Custom;
}

const QString CPluginTemplateBase::Protocol() const
{
    // TODO: add protocol
    return QString();
}

const QString CPluginTemplateBase::Name() const
{
    // TODO: add plugin name
    return "Template base";
}

const QString CPluginTemplateBase::Description() const
{
    // TODO: add description
    return "Template base plugin";
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
    return new COperateTemplateBase(this);
}