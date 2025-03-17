// Author: Kang Lin <kl222@126.com>

/*!
 *  \~english \defgroup VIEWER_APP Rabbit remote control programe
 *  \brief Rabbit remote control programe
 *  
 *  \~chinese \defgroup VIEWER_APP 玉兔远程控制程序
 *  \brief 玉兔远程控制程序
 *  \~
 *  \ingroup APP
 *  
 */

#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QDebug>
#include <QtGlobal>
#include <QSharedPointer>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QRegularExpression>
#endif
#if defined(Q_OS_ANDROID) && (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QtAndroid>
#endif

#include "RabbitCommonTools.h"

#ifdef HAVE_UPDATE
#include "FrmUpdater.h"
#endif
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "App.Main")

int main(int argc, char *argv[])
{
    int nRet = 0;

    //qputenv("QT_MEDIA_BACKEND", "ffmpeg");

#if (defined(Q_OS_LINUX) && !defined(Q_OA_ANDROID) \
    && (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)))
    /* 修复使用 Qt6 时，最大化时，工具栏位置错误。
       现在很多 linux 用 wayland 或 wayland-egl 作为桌面显示，这样会出现一个问题，
       由于没有坐标系统，导致无边框窗体无法拖动和定位（即 QWidge::move 失效）。
      （Qt6 开始强制默认优先用 wayland ，Qt5 默认有 xcb 则优先用 xcb），
       所以需要在 main 函数最前面加一行 `qputenv("QT_QPA_PLATFORM", "xcb")`;
    */
    QString szPlatform = QString::fromLocal8Bit(qgetenv("QT_QPA_PLATFORM"));
    if(szPlatform.isEmpty()
        || -1 != szPlatform.indexOf(QRegularExpression("wayland.*")))
        qputenv("QT_QPA_PLATFORM", "xcb");
    qInfo(log) << "QT_QPA_PLATFORM:" << szPlatform << "\nCurrent Qt Platform is:"
               << QString::fromLocal8Bit(qgetenv("QT_QPA_PLATFORM"))
               << "; This can be modified with the environment variables QT_QPA_PLATFORM:\n"
               << "export QT_QPA_PLATFORM=xcb\n Optional: xcb; vnc";
#endif
#if (QT_VERSION > QT_VERSION_CHECK(5,6,0)) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#if defined(Q_OS_ANDROID) && (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QtAndroid::hideSplashScreen();
#endif

//#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
//    Q_INIT_RESOURCE(translations_RabbitRemoteControlApp);
//#endif

    QApplication::setApplicationVersion(RabbitRemoteControl_VERSION);
    QApplication::setApplicationName("RabbitRemoteControl");
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QApplication::setDesktopFileName(QLatin1String("RabbitRemoteControl.desktop"));
#endif

    QApplication app(argc, argv);

    RabbitCommon::CTools::Instance()->Init();

    qInfo(log) << app.applicationName() + " " + app.applicationVersion()
                      + " " + QObject::tr("Start") + " ......"
               << "\n" << app.arguments();

    QSharedPointer<QTranslator> tApp =
        RabbitCommon::CTools::Instance()->InstallTranslator("RabbitRemoteControlApp");

    app.setApplicationDisplayName(QObject::tr("Rabbit Remote Control"));
    app.setOrganizationName(QObject::tr("Kang Lin Studio"));

#ifdef HAVE_UPDATE
    // Check update version
    QSharedPointer<CFrmUpdater> pUpdate(new CFrmUpdater());
    if(pUpdate) {
        QIcon icon = QIcon::fromTheme("app");
        if(!icon.isNull())
        {
            auto sizeList = icon.availableSizes();
            if(!sizeList.isEmpty()){
                QPixmap p = icon.pixmap(*sizeList.begin());
                pUpdate->SetTitle(p.toImage());
            }
        }
        if(app.arguments().length() > 1) {
            try{
                pUpdate->GenerateUpdateJson();
                pUpdate->GenerateUpdateXml();
            } catch(...) {
                qCritical(log) << "Generate update fail";
            }

            qInfo(log) << app.applicationName() + " " + app.applicationVersion()
                              + " " + QObject::tr("Generate update json file End");
            return 0;
        }
    } else {
        qCritical(log) << "new CFrmUpdater() fail";
    }
#endif

    MainWindow* w = new MainWindow();
    try {
        //w->setWindowIcon(QIcon::themeName("app"));
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

        nRet = app.exec();
    } catch (std::exception &e) {
        qCritical(log) << "exception:" << e.what();
    } catch(...) {
        qCritical(log) << "exception:";
    }

#ifndef BUILD_QUIWidget
    delete w;
#endif

    RabbitCommon::CTools::Instance()->Clean();
    if(tApp)
        RabbitCommon::CTools::Instance()->RemoveTranslator(tApp);
//#if defined (_DEBUG) || !defined(BUILD_SHARED_LIBS)
//    Q_CLEANUP_RESOURCE(translations_RabbitRemoteControlApp);
//#endif
    
    qInfo(log) << app.applicationName() + " " + app.applicationVersion() + " " + QObject::tr("End");
    return nRet;
}
