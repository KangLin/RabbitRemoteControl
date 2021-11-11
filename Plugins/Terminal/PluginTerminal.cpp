#include "PluginTerminal.h"
#include "RabbitCommonDir.h"
#include "ConnecterTerminal.h"

#include <QDebug>

CPluginTerminal::CPluginTerminal(QObject *parent)
    : CPluginViewer(parent)
{
}

CPluginTerminal::~CPluginTerminal()
{
    qDebug() << "CPluginFactoryTerminal::~CPluginFactoryTerminal()";
}

const QString CPluginTerminal::Protol() const
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
    return QIcon(":/image/Console");
}

CConnecter *CPluginTerminal::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        return new CConnecterTerminal(this);
    }
    return nullptr;
}
