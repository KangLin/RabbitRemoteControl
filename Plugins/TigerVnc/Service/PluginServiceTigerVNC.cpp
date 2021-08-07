#include "PluginServiceTigerVNC.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include <QDebug>
#include <QApplication>
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include "ServiceTigerVNC.h"

CPluginServiceTigerVNC::CPluginServiceTigerVNC(QObject *parent) : CPluginService(parent)
{
    // Load translator resource
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_PluginServiceTigerVnc);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/PluginServiceTigerVnc_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
    
    //rfb::SecurityClient::setDefaults();
    
    rfb::initStdIOLoggers();
#ifdef WIN32
    rfb::initFileLogger("C:\\temp\\vncviewer.log");
#else
    rfb::initFileLogger("/tmp/vncviewer.log");
#endif
    rfb::LogWriter::setLogParams("*:stderr:100");
    
}

CPluginServiceTigerVNC::~CPluginServiceTigerVNC()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CManageConnectTigerVnc::~CManageConnectTigerVnc()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_CLEANUP_RESOURCE(translations_PluginServiceTigerVnc);
#endif
}

const QString CPluginServiceTigerVNC::Protol() const
{
    return "RFB";
}

const QString CPluginServiceTigerVNC::Name() const
{
    return tr("TigerVNC");
}

const QString CPluginServiceTigerVNC::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

CService *CPluginServiceTigerVNC::NewService()
{
    return new CServiceTigerVNC(this);
}
