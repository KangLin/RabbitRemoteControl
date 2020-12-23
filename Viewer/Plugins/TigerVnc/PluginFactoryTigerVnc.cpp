//! @author: Kang Lin(kl222@126.com)

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
            + "/" + Name() + "_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
    
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

QString CPluginFactoryTigerVnc::Name()
{
    return "TigerVnc";
}

QString CPluginFactoryTigerVnc::Description()
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

QString CPluginFactoryTigerVnc::Protol()
{
    return "VNC";
}

CConnecter* CPluginFactoryTigerVnc::CreateConnecter(const QString &szProtol)
{
    if(Protol() == szProtol)
    {   
        CConnecter* p = new CConnecterTigerVnc(this);
        return p;
    }
    return nullptr;
}

QIcon CPluginFactoryTigerVnc::Icon()
{
    return QIcon(":/image/Connect");
}
