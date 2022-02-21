/*!
 *  \~english \defgroup ServiceConfigure Rabbit remote control service configure programe
 *  \brief Rabbit remote control service configure programe
 *  
 *  \~chinese \defgroup ServiceConfigure 玉兔远程控制服务配置程序
 *  \brief 玉兔远程控制服务配置程序
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
#include "RabbitCommonLog.h"
#ifdef HAVE_UPDATE
#include "FrmUpdater/FrmUpdater.h"
#endif
#ifdef BUILD_QUIWidget
    #include "QUIWidget/QUIWidget.h"
#endif

int main(int argc, char *argv[])
{
    int nRet = 0;
    QApplication::setApplicationVersion(BUILD_VERSION);
    QApplication::setApplicationName("RabbitRemoteControlService");
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QApplication::setDesktopFileName(QLatin1String("RabbitRemoteControlServiceConfigure.desktop"));
#endif

    QApplication a(argc, argv);

    RabbitCommon::CTools::Instance()->Init();

    // Install translator
    QTranslator tApp;
    QString szFile = RabbitCommon::CDir::Instance()->GetDirTranslations()
            + QDir::separator() + "RabbitRemoteControlService_"
            + QLocale::system().name() + ".qm";
    tApp.load(szFile);
    a.installTranslator(&tApp);
    LOG_MODEL_INFO("Main", "Language: %s; %s",
                   QLocale::system().name().toStdString().c_str(),
                   szFile.toStdString().c_str());

    a.setApplicationDisplayName(QObject::tr("Rabbit remote control service configure"));
    a.setOrganizationName(QObject::tr("Kang Lin studio"));
    
    MainWindow* w = new MainWindow();
    w->setWindowTitle(a.applicationDisplayName());
    try {
        //w->setWindowIcon(QIcon(":/image/App"));
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
        LOG_MODEL_ERROR("main", "exception: %s", e.what());
    } catch(...) {
        LOG_MODEL_ERROR("main", "exception");
    }

#ifndef BUILD_QUIWidget
    delete w;
#endif

    RabbitCommon::CTools::Instance()->Clean();
    a.removeTranslator(&tApp);
    
    return nRet;
}
