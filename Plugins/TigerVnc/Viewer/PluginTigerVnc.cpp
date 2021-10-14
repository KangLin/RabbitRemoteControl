// Author: Kang Lin <kl222@126.com>

#include "PluginTigerVnc.h"
#include "ConnecterTigerVnc.h"
#include <rfb/LogWriter.h>
#include <rfb/Logger_stdio.h>
#include <QDebug>
#include <QApplication>
#include "RabbitCommonDir.h"

static bool initlog = false;
CPluginTigerVnc::CPluginTigerVnc(QObject *parent)
    : CPluginViewer(parent)
{
    //! [Initialize resorce]
    
    // Load translator resource
    /*
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_TigerVnc);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/TigerVnc_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);//*/
    
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
    //! [Initialize resorce]
    
    m_pThread = new CPluginThread();
    if(m_pThread)
    {
        bool check = connect(m_pThread, SIGNAL(finished()),
                           m_pThread, SLOT(deleteLater()));
        Q_ASSERT(check);
        m_pThread->start();
    }
}

CPluginTigerVnc::~CPluginTigerVnc()
{
    //! [Clean resource]
    qDebug() << "CManageConnectTigerVnc::~CManageConnectTigerVnc()";
    /*
    qApp->removeTranslator(&m_Translator);
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_CLEANUP_RESOURCE(translations_TigerVnc);
#endif//*/
    //! [Clean resource]
}

const QString CPluginTigerVnc::Name() const
{
    return tr("TigerVNC");
}

const QString CPluginTigerVnc::Description() const
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

const QString CPluginTigerVnc::Protol() const
{
    return "RFB";
}

CConnecter* CPluginTigerVnc::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {
        CConnecterTigerVnc* pConnecter = new CConnecterTigerVnc(this);
        if(pConnecter && m_pThread)
        {
            bool check = connect(pConnecter, SIGNAL(sigConnect(CConnecter*)),
                           m_pThread, SIGNAL(sigConnect(CConnecter*)));
            Q_ASSERT(check);
            check = connect(pConnecter, SIGNAL(sigDisconnect(CConnecter*)),
                         m_pThread, SIGNAL(sigDisconnect(CConnecter*)));
            Q_ASSERT(check);
        }
        return pConnecter;
    }
    return nullptr;
}

const QIcon CPluginTigerVnc::Icon() const
{
    return QIcon(":/image/Connect");
}
