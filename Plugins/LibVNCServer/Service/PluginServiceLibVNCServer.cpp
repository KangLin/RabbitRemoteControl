#include "PluginServiceLibVNCServer.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include <QLocale>
#include <QDebug>
#include <QApplication>
#include "ServiceLibVNCServer.h"

CPluginServiceLibVNCServer::CPluginServiceLibVNCServer(QObject *parent)
    : CPluginService(parent)
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_PluginServiceLibVNCServer);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/PluginServiceLibVNCServer_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
}

CPluginServiceLibVNCServer::~CPluginServiceLibVNCServer()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CPluginServiceLibVNCServer::~CPluginServiceLibVNCServer()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_CLEANUP_RESOURCE(translations_PluginServiceLibVNCServer);
#endif
}

const QString CPluginServiceLibVNCServer::Protol() const
{
    return "RFB";
}

const QString CPluginServiceLibVNCServer::Name() const
{
    return tr("LibVNCServer");
}

const QString CPluginServiceLibVNCServer::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

CService *CPluginServiceLibVNCServer::NewService()
{
    return new CServiceLibVNCServer(this);
}
