#include "PluginFactoryTelnet.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include "ConnecterTelnet.h"

#include <QCoreApplication>
#include <QLocale>
#include <QDebug>

CPluginFactoryTelnet::CPluginFactoryTelnet()
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_Telnet);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/" + Name() + "_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
}

CPluginFactoryTelnet::~CPluginFactoryTelnet()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CPluginFactoryTerminal::~CPluginFactoryTerminal()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_Telnet);
#endif
}

const QString CPluginFactoryTelnet::Protol() const
{
    return "Telnet";
}

const QString CPluginFactoryTelnet::Name() const
{
    return "Telnet";
}

const QString CPluginFactoryTelnet::Description() const
{
    return tr("Telnet: net terminal");
}

const QIcon CPluginFactoryTelnet::Icon() const
{
    return QIcon(":/image/Console");
}

CConnecter *CPluginFactoryTelnet::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        CConnecter* p = new CConnecterTelnet(this);
        return p;
    }
    return nullptr;
}
