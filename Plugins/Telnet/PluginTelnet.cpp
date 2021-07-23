#include "PluginTelnet.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include "ConnecterTelnet.h"

#include <QCoreApplication>
#include <QLocale>
#include <QDebug>

CPluginTelnet::CPluginTelnet(QObject *parent)
    : CPlugin(parent)
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

CPluginTelnet::~CPluginTelnet()
{
    qDebug() << "CPluginFactoryTelnet::~CPluginFactoryTelnet()";
    qApp->removeTranslator(&m_Translator);    
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_CLEANUP_RESOURCE(translations_Telnet);
#endif
}

const QString CPluginTelnet::Protol() const
{
    return "Telnet";
}

const QString CPluginTelnet::Name() const
{
    return tr("Telnet");
}

const QString CPluginTelnet::Description() const
{
    return tr("Telnet: net terminal");
}

const QIcon CPluginTelnet::Icon() const
{
    return QIcon(":/image/Console");
}

CConnecter *CPluginTelnet::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        return new CConnecterTelnet(this);
    }
    return nullptr;
}
