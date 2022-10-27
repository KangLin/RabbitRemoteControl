// Author: Kang Lin <kl222@126.com>

/*!
 *  \~english \defgroup ServiceConfigure Rabbit remote control service configure programe
 *  \brief Rabbit remote control service configure programe
 *  \details Provides a graphical interface for configuring remote control services.
 *  It includes the following features:  
 *   - Configure service parameters
 *   - In system service mode, start and stop services
 *   - In user mode, start and stop services (can be used to debug services)
 *   
 *  \~chinese \defgroup ServiceConfigure 玉兔远程控制服务配置程序
 *  \brief 玉兔远程控制服务配置程序
 *  \details 提供图形化界面用于配置远程控制服务。
 *           它包括以下功能：  
 *           - 配置服务参数
 *           - 在系统服务模式下，开始、停止服务
 *           - 在用户模式下，开始、停止服务（可以用于调试服务）
 *           
 *  \~
 *  \ingroup APP
 */

#include "MainWindow.h"

#include <QApplication>
#include <QSettings>
#include <QDebug>
#include <QtGlobal>
#include <QSharedPointer>
#if defined(Q_OS_ANDROID)
    #include <QtAndroid>
#endif

#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#ifdef HAVE_UPDATE
#include "FrmUpdater/FrmUpdater.h"
#endif
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(App, "App")

int main(int argc, char *argv[])
{
    RabbitCommon::CTools::EnableCoreDump();
    int nRet = 0;
    QApplication::setApplicationVersion(RabbitRemoteControl_VERSION);
    QApplication::setApplicationName("RabbitRemoteControlService");
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QApplication::setDesktopFileName(QLatin1String("RabbitRemoteControlServiceConfigure.desktop"));
#endif

    QApplication a(argc, argv);

    RabbitCommon::CTools::Instance()->Init();
    // Install translator
    QTranslator tApp;
    QString szFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + QDir::separator() + "RabbitRemoteControlServiceConfigure_"
            + QLocale::system().name() + ".qm";
    tApp.load(szFile);
    if(a.installTranslator(&tApp))
        qInfo(App) << "Language:" << QLocale::system() << "File:" << szFile;
    else
        qCritical(App) << "Language:" << QLocale::system() << "File:" << szFile;

    a.setApplicationDisplayName(QObject::tr("Rabbit remote control service configure"));
    a.setOrganizationName(QObject::tr("Kang Lin studio"));
    
    CMainWindow* w = new CMainWindow();
    w->setWindowTitle(a.applicationDisplayName());
    try {
        //w->setWindowIcon(QIcon::fromTheme("app"));
        //w->setWindowTitle(a.applicationDisplayName());
        
#ifdef BUILD_QUIWidget
        QSharedPointer<QUIWidget> quiwidget(new QUIWidget(nullptr, true));
        bool check = quiwidget->connect(w, SIGNAL(sigFullScreen()),
                                        SLOT(showFullScreen()));
        Q_ASSERT(check);
        check = quiwidget->connect(w, SIGNAL(sigShowNormal()),
                                   SLOT(showNormal()));
        Q_ASSERT(check);
        //quiwidget.setPixmap(QUIWidget::Lab_Ico, ":/image/App");
        //quiwidget.setTitle(a.applicationDisplayName());
        quiwidget->setMainWidget(w);
        quiwidget->show();
#else
        w->show();
#endif

        nRet = a.exec();
    } catch (std::exception &e) {
        qCritical(App) << "exception:" << e.what();
    } catch(...) {
        qCritical(App) << "exception";
    }

#ifndef BUILD_QUIWidget
    delete w;
#endif

    RabbitCommon::CTools::Instance()->Clean();
    a.removeTranslator(&tApp);
    
    return nRet;
}
