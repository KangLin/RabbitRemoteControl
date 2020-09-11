#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QDebug>
#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"

int main(int argc, char *argv[])
{
#if (QT_VERSION > QT_VERSION_CHECK(5,6,0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if defined(Q_OS_ANDROID) && QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QtAndroid::hideSplashScreen();
#endif
    
#ifdef _DEBUG || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_RabbitRemoteControl);
#endif
    
    QApplication a(argc, argv);
    //a.setApplicationName("RabbitRemoteControl");
    
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    
    QTranslator tApp;
    tApp.load(RabbitCommon::CDir::Instance()->GetDirTranslations()
              + QDir::separator() + a.applicationName() + "_"
              + QLocale::system().name() + ".qm");
    a.installTranslator(&tApp);
    qInfo() << "Language:" << QLocale::system().name();
    
    RabbitCommon::CTools::Instance()->Init();
    
    a.setApplicationDisplayName(QObject::tr("Rabbit Remote Control"));
    
    MainWindow w;
    w.setWindowIcon(QIcon(":/image/App"));
    w.show();
    int nRet = a.exec();
    
#ifdef _DEBUG || !defined(BUILD_SHARED_LIBS)
    Q_INIT_RESOURCE(translations_RabbitRemoteControl);
#endif
    
    return nRet;
}
