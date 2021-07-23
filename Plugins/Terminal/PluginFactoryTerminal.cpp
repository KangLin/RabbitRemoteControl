#include "PluginFactoryTerminal.h"
#include "RabbitCommonDir.h"
#include "ConnecterTerminal.h"

#include <QCoreApplication>
#include <QLocale>
#include <QDebug>

CPluginFactoryTerminal::CPluginFactoryTerminal(QObject *parent)
    : CPlugin(parent)
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_PluginViewerTerminal);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/PluginViewerTerminal_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
}

CPluginFactoryTerminal::~CPluginFactoryTerminal()
{
    qDebug() << "CPluginFactoryTerminal::~CPluginFactoryTerminal()";
    qApp->removeTranslator(&m_Translator);
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_CLEANUP_RESOURCE(translations_PluginViewerTerminal);
#endif
}

const QString CPluginFactoryTerminal::Protol() const
{
    return "Terminal";
}

const QString CPluginFactoryTerminal::Name() const
{
    return tr("Terminal");
}

const QString CPluginFactoryTerminal::Description() const
{
    return tr("Terminal: native(shell) and remote(telnet, etc) terminal");
}

const QIcon CPluginFactoryTerminal::Icon() const
{
    return QIcon(":/image/Console");
}

CConnecter *CPluginFactoryTerminal::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        return new CConnecterTerminal(this);
    }
    return nullptr;
}
