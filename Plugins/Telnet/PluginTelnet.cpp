#include "PluginTelnet.h"
#include "OperateTelnet.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Plugin.Telnet")

CPluginTelnet::CPluginTelnet(QObject *parent)
    : CPlugin(parent)
{
}

CPluginTelnet::~CPluginTelnet()
{
    qDebug(log) << Q_FUNC_INFO;
}

const QString CPluginTelnet::Protocol() const
{
    return "Telnet";
}

const QString CPluginTelnet::Name() const
{
    return "Telnet";
}

const QString CPluginTelnet::DisplayName() const
{
    return tr("Telnet");
}

const QString CPluginTelnet::Description() const
{
    return tr("Telnet: It is not safe. Please use SSH in the production environment.");
}

const QIcon CPluginTelnet::Icon() const
{
    return QIcon::fromTheme("console");
}

const CPlugin::TYPE CPluginTelnet::Type() const
{
    return TYPE::Terminal;
}

const QString CPluginTelnet::Version() const
{
    return PluginTelnet_VERSION;
}

COperate *CPluginTelnet::OnCreateOperate(const QString &szId)
{
    if(Id() == szId)
        return new COperateTelnet(this);
    return nullptr;
}
