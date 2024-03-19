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
#if defined(Q_OS_ANDROID) && (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <QtAndroid>
#endif

#include "RabbitCommonTools.h"

#ifdef HAVE_UPDATE
#include "FrmUpdater/FrmUpdater.h"
#endif
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif

#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "App.Main")

int main(int argc, char *argv[])
{
    RabbitCommon::CTools::EnableCoreDump();

    int nRet = 0;
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

    QApplication a(argc, argv);

    RabbitCommon::CTools::Instance()->Init();

    qInfo(log) << a.applicationName() + " " + a.applicationVersion() + " " + QObject::tr("Start");

    QSharedPointer<QTranslator> tApp =
        RabbitCommon::CTools::Instance()->InstallTranslator("RabbitRemoteControlApp");

    a.setApplicationDisplayName(QObject::tr("Rabbit Remote Control"));
    a.setOrganizationName(QObject::tr("Kang Lin Studio"));

#ifdef HAVE_UPDATE
    // Check update version
    QSharedPointer<CFrmUpdater> pUpdate(new CFrmUpdater());
    QIcon icon = QIcon::fromTheme("app");
    if(!icon.isNull())
    {
        auto sizeList = icon.availableSizes();
        if(!sizeList.isEmpty()){
            QPixmap p = icon.pixmap(*sizeList.begin());
            pUpdate->SetTitle(p.toImage());
        }
    }
    if(a.arguments().length() > 1) {
        pUpdate->GenerateUpdateJson();
        pUpdate->GenerateUpdateXml();
        return 0;
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

        nRet = a.exec();
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
    
    qInfo(log) << a.applicationName() + " " + a.applicationVersion() + " " + QObject::tr("End");
    return nRet;
}
