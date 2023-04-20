#include "PluginTerminal.h"
#include "RabbitCommonDir.h"
#include "ConnecterPluginTerminal.h"

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(Terminal, "Client.Terminal")

CPluginTerminal::CPluginTerminal(QObject *parent)
    : CPluginClient(parent)
{
}

CPluginTerminal::~CPluginTerminal()
{
    qDebug(Terminal) << "CPluginFactoryTerminal::~CPluginFactoryTerminal()";
}

const QString CPluginTerminal::Protocol() const
{
    return "Terminal";
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
    return tr("Terminal: native(shell) and remote(telnet, etc) terminal");
}

const QIcon CPluginTerminal::Icon() const
{
    return QIcon::fromTheme("console");
}

CConnecter *CPluginTerminal::CreateConnecter(const QString &szProtocol)
{
    if(Id() == szProtocol)
    {   
        return new CConnecterPluginTerminal(this);
    }
    return nullptr;
}
