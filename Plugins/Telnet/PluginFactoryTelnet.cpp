#include "PluginFactoryTelnet.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include "ConnecterTelnet.h"

#include <QCoreApplication>
#include <QLocale>
#include <QDebug>

CPluginFactoryTelnet::CPluginFactoryTelnet(QObject *parent)
    : CPluginFactory(parent)
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_Telnet);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/Telnet_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
}

CPluginFactoryTelnet::~CPluginFactoryTelnet()
{
    qDebug() << "CPluginFactoryTelnet::~CPluginFactoryTelnet()";
    qApp->removeTranslator(&m_Translator);    
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_CLEANUP_RESOURCE(translations_Telnet);
#endif
}

const QString CPluginFactoryTelnet::Protol() const
{
    return "Telnet";
}

const QString CPluginFactoryTelnet::Name() const
{
    return tr("Telnet");
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
        return new CConnecterTelnet(this);
    }
    return nullptr;
}
