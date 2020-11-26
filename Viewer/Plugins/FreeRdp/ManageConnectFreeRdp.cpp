#include "ManageConnectFreeRdp.h"
#include <QDebug>
#include <QApplication>
#include "RabbitCommonDir.h"
#include "ConnecterFreeRdp.h"

CManageConnectFreeRdp::CManageConnectFreeRdp(QObject *parent)
    : CManageConnecter(parent)
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

CManageConnectFreeRdp::~CManageConnectFreeRdp()
{
    qApp->removeTranslator(&m_Translator);
    qDebug() << "CManageConnectFreeRdp::~CManageConnectFreeRdp()";
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_FreeRdp);
#endif
}

QString CManageConnectFreeRdp::Name()
{
    return "FreeRdp";
}

QString CManageConnectFreeRdp::Description()
{
    return tr("RDP(Windows remote desktop protol): Access remote desktops such as windows.");
}

QString CManageConnectFreeRdp::Protol()
{
    return "RDP";
}

CConnecter* CManageConnectFreeRdp::CreateConnecter(const QString &szProtol)
{
    if(Protol() == szProtol)
    {   
        CConnecter* p = new CConnecterFreeRdp(this);
        return p;
    }
    return nullptr;
}
