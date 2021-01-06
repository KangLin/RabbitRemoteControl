//! @author: Kang Lin(kl222@126.com)

#include "PluginFactoryFreeRdp.h"
#include <QDebug>
#include <QApplication>
#include "RabbitCommonDir.h"
#include "ConnecterFreeRdp.h"

CPluginFactoryFreeRdp::CPluginFactoryFreeRdp(QObject *parent)
    : CPluginFactory(parent)
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_FreeRdp);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/" + Name() + "_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
}

CPluginFactoryFreeRdp::~CPluginFactoryFreeRdp()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CManageConnectFreeRdp::~CManageConnectFreeRdp()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_FreeRdp);
#endif
}

QString CPluginFactoryFreeRdp::Name()
{
    return "FreeRdp";
}

QString CPluginFactoryFreeRdp::Description()
{
    return tr("RDP(Windows remote desktop protol): Access remote desktops such as windows.");
}

QString CPluginFactoryFreeRdp::Protol()
{
    return "RDP";
}

QIcon CPluginFactoryFreeRdp::Icon()
{
    return QIcon(":/image/Windows");
}

CConnecter* CPluginFactoryFreeRdp::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        CConnecter* p = new CConnecterFreeRdp(this);
        return p;
    }
    return nullptr;
}
