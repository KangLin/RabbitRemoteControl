#include "ManageConnectTigerVnc.h"
#include "ConnecterTigerVnc.h"
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include <QDebug>
#include <QApplication>
#include "RabbitCommonDir.h"

static bool initlog = false;
CManageConnectTigerVnc::CManageConnectTigerVnc(QObject *parent)
    : CManageConnecter(parent)
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

CManageConnectTigerVnc::~CManageConnectTigerVnc()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CManageConnectTigerVnc::~CManageConnectTigerVnc()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_TigerVnc);
#endif
}

QString CManageConnectTigerVnc::Name()
{
    return "TigerVnc";
}

QString CManageConnectTigerVnc::Description()
{
    return "Access remote desktops such as unix/linux, windows, etc.";
}

QString CManageConnectTigerVnc::Protol()
{
    return "VNC";
}

CConnecter* CManageConnectTigerVnc::CreateConnecter(const QString &szProtol)
{
    if(Protol() == szProtol)
    {   
        CConnecter* p = new CConnecterTigerVnc(this);
        return p;
    }
    return nullptr;
}
