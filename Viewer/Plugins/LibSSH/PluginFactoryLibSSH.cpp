#include "PluginFactoryLibSSH.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include "ConnecterSSH.h"

#include <QCoreApplication>
#include <QLocale>
#include <QDebug>

CPluginFactoryLibSSH::CPluginFactoryLibSSH()
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_LibSSH);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/LibSSH_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
}

CPluginFactoryLibSSH::~CPluginFactoryLibSSH()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CPluginFactoryLibSSH::~CPluginFactoryLibSSH()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_LibSSH);
#endif
}

const QString CPluginFactoryLibSSH::Protol() const
{
    return "SSH";
}

const QString CPluginFactoryLibSSH::Name() const
{
    return tr("LibSSH");
}

const QString CPluginFactoryLibSSH::Description() const
{
    return tr("SSH: you can remotely execute programs, transfer files, use a secure and transparent tunnel, manage public keys and much more ...");
}

const QIcon CPluginFactoryLibSSH::Icon() const
{
    return QIcon(":/image/SSH");
}

CConnecter *CPluginFactoryLibSSH::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
        return new CConnecterSSH(this);
    return nullptr;
}
