#include "PluginTerminal.h"
#include "RabbitCommonDir.h"
#include "Terminal.h"

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Plugin.Terminal")

CPluginTerminal::CPluginTerminal(QObject *parent)
    : CPlugin(parent)
{
    qDebug(log) << Q_FUNC_INFO;
}

CPluginTerminal::~CPluginTerminal()
{
    qDebug(log) << Q_FUNC_INFO;
}

const QString CPluginTerminal::Protocol() const
{
    return QString();
}

const QString CPluginTerminal::Name() const
{
    return "Terminal";
}

const QString CPluginTerminal::DisplayName() const
{
    return tr("Terminal");
}

const QString CPluginTerminal::Description() const
{
    return tr("Terminal: Native(shell) terminal.") + "\n"
           + tr("It uses QTermWidget: https://github.com/KangLin/qtermwidget");
}

const QIcon CPluginTerminal::Icon() const
{
    return QIcon::fromTheme("console");
}

const CPlugin::TYPE CPluginTerminal::Type() const
{
    return TYPE::Terminal;
}

const QString CPluginTerminal::Version() const
{
    return 0;
}

const QString CPluginTerminal::Details() const
{
    QString szDetails;
    szDetails = COperateTerminal::Details();
    return szDetails;
}

COperate *CPluginTerminal::OnCreateOperate(const QString &szId)
{
    if(Id() == szId)
    {
        return new CTerminal(this);
    }
    return nullptr;
}
