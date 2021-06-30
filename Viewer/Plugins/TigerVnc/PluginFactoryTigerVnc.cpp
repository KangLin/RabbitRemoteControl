//! @author: Kang Lin <kl222@126.com>

#include "PluginFactoryTigerVnc.h"
#include "ConnecterTigerVnc.h"
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include <QDebug>
#include <QApplication>
#include "RabbitCommonDir.h"

static bool initlog = false;
CPluginFactoryTigerVnc::CPluginFactoryTigerVnc(QObject *parent)
    : CPluginFactory(parent)
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_TigerVnc);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/TigerVnc_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
    
    //rfb::SecurityClient::setDefaults();
    if(!initlog)
    {
        rfb::initStdIOLoggers();
#ifdef WIN32
        rfb::initFileLogger("C:\\temp\\vncviewer.log");
#else
        rfb::initFileLogger("/tmp/vncviewer.log");
#endif
        rfb::LogWriter::setLogParams("*:stderr:100");
        initlog = true;
    }
    
}

CPluginFactoryTigerVnc::~CPluginFactoryTigerVnc()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CManageConnectTigerVnc::~CManageConnectTigerVnc()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_TigerVnc);
#endif
}

const QString CPluginFactoryTigerVnc::Name() const
{
    return tr("TigerVnc");
}

const QString CPluginFactoryTigerVnc::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

const QString CPluginFactoryTigerVnc::Protol() const
{
    return "RFB";
}

CConnecter* CPluginFactoryTigerVnc::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        return new CConnecterTigerVnc(this);
    }
    return nullptr;
}

const QIcon CPluginFactoryTigerVnc::Icon() const
{
    return QIcon(":/image/Connect");
}
