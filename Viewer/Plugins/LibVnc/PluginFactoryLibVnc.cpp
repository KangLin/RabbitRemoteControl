#include "PluginFactoryLibVnc.h"
#include "RabbitCommonDir.h"
#include "ConnecterLibVnc.h"
#include "QApplication"
#include <QDebug>

CPluginFactoryLibVnc::CPluginFactoryLibVnc(QObject *parent) : CPluginFactory(parent)
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_LibVnc);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/" + Name() + "_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
}

CPluginFactoryLibVnc::~CPluginFactoryLibVnc()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CPluginFactoryLibVnc::~CPluginFactoryLibVnc()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_LibVnc);
#endif
}

QString CPluginFactoryLibVnc::Name()
{
    return tr("LibVnc");
}

QString CPluginFactoryLibVnc::Description()
{
    return tr("VNC(Virtual Network Console): Access remote desktops such as unix/linux, windows, etc.") + "\n"
            + tr("VNC is a set of programs using the RFB (Remote Frame Buffer) protocol.");
}

QString CPluginFactoryLibVnc::Protol()
{
    return "RFB";
}

QIcon CPluginFactoryLibVnc::Icon()
{
    return QIcon(":/image/Connect");
}

CConnecter *CPluginFactoryLibVnc::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        CConnecter* p = new CConnecterLibVnc(this);
        return p;
    }
    return nullptr;
}
