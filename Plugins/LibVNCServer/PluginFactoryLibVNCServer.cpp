#include "PluginFactoryLibVNCServer.h"
#include "RabbitCommonDir.h"
#include "ConnecterLibVNCServer.h"
#include "QApplication"
#include <QDebug>

CPluginFactoryLibVNCServer::CPluginFactoryLibVNCServer(QObject *parent) : CPlugin(parent)
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_LibVNCServer);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/LibVNCServer_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
}

CPluginFactoryLibVNCServer::~CPluginFactoryLibVNCServer()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CPluginFactoryLibVNCServer::~CPluginFactoryLibVNCServer()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_CLEANUP_RESOURCE(translations_LibVNCServer);
#endif
}

const QString CPluginFactoryLibVNCServer::Name() const
{
    return tr("LibVNCServer");
}

const QString CPluginFactoryLibVNCServer::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

const QString CPluginFactoryLibVNCServer::Protol() const
{
    return "RFB";
}

const QIcon CPluginFactoryLibVNCServer::Icon() const
{
    return QIcon(":/image/Connect");
}

CConnecter *CPluginFactoryLibVNCServer::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        return new CConnecterLibVNCServer(this);
    }
    return nullptr;
}
