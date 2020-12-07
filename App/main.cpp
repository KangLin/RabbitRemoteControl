#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QDebug>
#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#include "FrmUpdater/FrmUpdater.h"
#include "log4cplus/logger.h"
#include "log4cplus/configurator.h"
#include "log4cplus/loggingmacros.h"
#include <log4cplus/log4cplus.h>

using namespace log4cplus;
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
    
    QApplication a(argc, argv);
    a.setApplicationName("RabbitRemoteControl");
    
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);

    // Init logger    
    log4cplus::initialize();
    QString szLogConfig = RabbitCommon::CDir::Instance()->GetDirConfig() + QDir::separator() + "log4config.conf";
    szLogConfig = set.value("Log/ConfigFile", szLogConfig).toString();
    log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(szLogConfig.toStdString().c_str()));

//    //以下两行将root logger配置为ConsoleAppender和SimpleLayout
//    log4cplus::BasicConfigurator config;
//    config.configure();
    
//    //第1步：创建ConsoleAppender
//    log4cplus::SharedAppenderPtr appender(new log4cplus::ConsoleAppender());
 
//    //第2步：设置Appender的名称和输出格式（SimpleLayout）
//    appender->setName(LOG4CPLUS_TEXT("console"));
//    appender->setLayout(std::unique_ptr<log4cplus::Layout>(new log4cplus::SimpleLayout));
//    log4cplus::Logger logger = log4cplus::Logger::getInstance("test");
//    logger.setLogLevel(log4cplus::INFO_LOG_LEVEL);
 
//    //第4步：为Logger实例添加ConsoleAppender
//    logger.addAppender(appender);
//    LOG4CPLUS_ERROR(log4cplus::Logger::getRoot(), "test..............");
    
    // Install translator
    QTranslator tApp;
    tApp.load(RabbitCommon::CDir::Instance()->GetDirTranslations()
              + QDir::separator() + a.applicationName() + "App_"
              + QLocale::system().name() + ".qm");
    a.installTranslator(&tApp);
    qInfo() << "Language:" << QLocale::system().name();
    
    RabbitCommon::CTools::Instance()->Init();
    
    a.setApplicationDisplayName(QObject::tr("Rabbit Remote Control"));
    
    // Check update version
    CFrmUpdater *pUpdate = new CFrmUpdater();
    pUpdate->SetTitle(QImage(":/image/App"));
    if(!pUpdate->GenerateUpdateXml()) 
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
