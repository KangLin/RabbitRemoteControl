//! @author: Kang Lin(kl222@126.com)

#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QDebug>
#include <QtGlobal>

#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonLog.h"
#include "FrmUpdater/FrmUpdater.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION > QT_VERSION_CHECK(5,6,0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if defined(Q_OS_ANDROID) && QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QtAndroid::hideSplashScreen();
#endif
    
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_RabbitRemoteControlApp);
#endif

    QApplication::setApplicationVersion(BUILD_VERSION);
    QApplication::setApplicationName("RabbitRemoteControl");
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QApplication::setDesktopFileName(QLatin1String("RabbitRemoteControl.desktop"));
#endif

    QApplication a(argc, argv);

    RabbitCommon::CTools::Instance()->Init();

    // Install translator
    QTranslator tApp;
    tApp.load(RabbitCommon::CDir::Instance()->GetDirTranslations()
              + QDir::separator() + "RabbitRemoteControlApp_"
              + QLocale::system().name() + ".qm");
    a.installTranslator(&tApp);
    LOG_MODEL_INFO("Main", "Language: %s", QLocale::system().name().toStdString().c_str());

    a.setApplicationDisplayName(QObject::tr("Rabbit Remote Control"));
    a.setOrganizationName(QObject::tr("Kang Lin studio"));
    
    // Check update version
    CFrmUpdater *pUpdate = new CFrmUpdater();
    pUpdate->SetTitle(QImage(":/image/App"));
    if(pUpdate->GenerateUpdateXml())
        LOG_MODEL_ERROR("main", "GenerateUpdateXml fail");
    else    
        return 0;

    MainWindow w;
    w.setWindowIcon(QIcon(":/image/App"));
    w.show();
    int nRet = a.exec();
    
    RabbitCommon::CTools::Instance()->Clean();
    a.removeTranslator(&tApp);
#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
    Q_CLEANUP_RESOURCE(translations_RabbitRemoteControlApp);
#endif
    
    return nRet;
}
