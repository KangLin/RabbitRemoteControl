// Author: Kang Lin <kl222@126.com>

#include "PluginFreeRdp.h"
#include <QDebug>
#include <QApplication>
#include "RabbitCommonDir.h"
#include "ConnecterFreeRdp.h"

CPluginFreeRdp::CPluginFreeRdp(QObject *parent)
    : CPlugin(parent)
{
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_FreeRdp);
#endif

    QString szTranslatorFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + "/FreeRdp_" + QLocale::system().name() + ".qm";
    if(!m_Translator.load(szTranslatorFile))
        qCritical() << "Open translator file fail:" << szTranslatorFile;
    qApp->installTranslator(&m_Translator);
}

CPluginFreeRdp::~CPluginFreeRdp()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CPluginFactoryFreeRdp::~CPluginFactoryFreeRdp()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_CLEANUP_RESOURCE(translations_FreeRdp);
#endif
}

const QString CPluginFreeRdp::Name() const
{
    return "FreeRdp";
}

const QString CPluginFreeRdp::Description() const
{
    return tr("RDP(Windows remote desktop protol): Access remote desktops such as windows.");
}

const QString CPluginFreeRdp::Protol() const
{
    return "RDP";
}

const QIcon CPluginFreeRdp::Icon() const
{
    return QIcon(":/image/Windows");
}

CConnecter* CPluginFreeRdp::CreateConnecter(const QString &szProtol)
{
    if(Id() == szProtol)
    {   
        return new CConnecterFreeRdp(this);
    }
    return nullptr;
}
