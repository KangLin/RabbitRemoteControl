// Author: Kang Lin <kl222@126.com>

#include <QStandardPaths>
#include <QCoreApplication>
#include <QSettings>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "WorkerFileTransfer.h"
#include "StatsAppUsage.h"

static Q_LOGGING_CATEGORY(log, "Stats.App")
CStatsAppUsage::CStatsAppUsage(const QString& szVersion,
                               const QString& szUrl, QObject *parent)
    : QObject{parent}
    , m_szVersion(szVersion)
    , m_szUrl(szUrl)
    , m_szExt("txt")
    , m_bRunOneDay(false)
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_szUrl.isEmpty()) {
        m_szUrl = "https://github.com/KangLin/"
                  + QCoreApplication::applicationName() + "/releases/download";
    }
    bool check = connect(&m_Timer, &QTimer::timeout,
                         this, &CStatsAppUsage::slotTimeout);
    Q_ASSERT(check);
    check = connect(&m_ThreadPool, &CThreadPool::sigRunning,
                    this, &CStatsAppUsage::Start);
    Q_ASSERT(check);
    check = connect(&m_ThreadPool, &CThreadPool::sigFinished,
                    this, &CStatsAppUsage::sigFinished);
    Q_ASSERT(check);
    m_ThreadPool.Start(CreateWorkerFileTransfer);
}

CStatsAppUsage::~CStatsAppUsage()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CStatsAppUsage::SetUrl(const QString &szUrl)
{
    m_szUrl = szUrl;
}

void CStatsAppUsage::SetVersion(const QString &szVersion)
{
    m_szVersion = szVersion;
}

void CStatsAppUsage::Start()
{
    Q_ASSERT_X(!m_szUrl.isEmpty(), "Stats.App",
               "Please call SetUrl() and SetVersion() first");
    m_tmStart = QDateTime::currentDateTime();
    Download("Start");
    StartDay();
    StartOnce();
    m_Timer.start(24 * 3600 * 1000);
}

void CStatsAppUsage::Stop()
{
    Download("End");
    StopDay();
    StopOnce();
    m_Timer.stop();
    m_ThreadPool.Stop();
}

void CStatsAppUsage::slotTimeout()
{
    RunOneDay();
}

/*!
 * \brief 程序安装后，同一用户所有启动计为一次
 */
void CStatsAppUsage::StartOnce()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    bool bOnce = set.value("Stats/App/Start/Once", false).toBool();
    if(bOnce)
        return;
    Download("StartOnce");
    set.setValue("Stats/App/Start/Once", true);
}

void CStatsAppUsage::StopOnce()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    bool bOnce = set.value("Stats/App/End/Once", false).toBool();
    if(bOnce)
        return;
    Download("EndOnce");
    set.setValue("Stats/App/End/Once", true);
}

/*!
 * \brief 程序每天所有启动计为一次
 */
void CStatsAppUsage::StartDay()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QDateTime tm = set.value("Stats/App/Start/Day").toDateTime();
    qDebug(log) << "daysTo:" << tm.daysTo(QDateTime::currentDateTime())
                << tm.isValid() << tm.isNull() << tm;
    if(tm.isValid() && !tm.isNull()
        && tm.daysTo(QDateTime::currentDateTime()) < 1)
        return;
    Download("StartDay");
    set.setValue("Stats/App/Start/Day", QDateTime::currentDateTime());
}

void CStatsAppUsage::StopDay()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QDateTime tm = set.value("Stats/App/End/Day").toDateTime();
    qDebug(log) << "daysTo:" << tm.daysTo(QDateTime::currentDateTime())
                << tm.isValid() << tm.isNull() << tm;
    if(tm.isValid() && !tm.isNull()
        && tm.daysTo(QDateTime::currentDateTime()) < 1)
        return;
    Download("EndDay");
    set.setValue("Stats/App/End/Day", QDateTime::currentDateTime());
}

/*!
 * \brief 运行超过一天的计为一次
 */
void CStatsAppUsage::RunOneDay()
{
    qDebug(log) << "secsTo:" << m_tmStart.secsTo(QDateTime::currentDateTime());
    if(m_tmStart.secsTo(QDateTime::currentDateTime()) >= 24 * 3600
        && !m_bRunOneDay) {
        m_bRunOneDay = true;
        Download("RunOneDay");
    }
}

void CStatsAppUsage::Download(const QString &szFile)
{
    QString file = "Stats" + szFile;
    if(m_szUrl.isEmpty()) return;
    QString szUrl = m_szUrl + "/";
    QString szVersion = m_szVersion;
    if(!szVersion.isEmpty()) {
        if(RabbitCommon::CTools::VersionValid(szVersion)) {
            auto map = RabbitCommon::CTools::GetVersion(szVersion);
            if(!map.value("PreRelease").isEmpty()
                || !map.value("Build").isEmpty()) {
                file += "Dev";
                static QRegularExpression regex("[-|+]");
                szVersion = szVersion.left(szVersion.indexOf(regex));
                qDebug(log) << "Version:" << szVersion;
            }
        } else
            szVersion = QString();
        if(!szVersion.isEmpty())
            szUrl += szVersion + "/";
    }

    szUrl += file;
    if(!m_szExt.isEmpty())
        szUrl += "." + m_szExt;
    qDebug(log) << Q_FUNC_INFO << szUrl;

    // Download
    CWorkerFileTransfer* pWorker =
        qobject_cast<CWorkerFileTransfer*>(m_ThreadPool.ChooseWorker());
    if(pWorker) {
        QUrl url(szUrl);
        QString szDownload;
        if(!url.fileName().isEmpty()) {
            szDownload = QStandardPaths::writableLocation(
                QStandardPaths::TempLocation);
            if(szDownload.right(1) != "/" && szDownload.right(1) != "\\")
                szDownload += QDir::separator();
            szDownload += QString("Rabbit") + QDir::separator()
                          + QCoreApplication::applicationName()
                          + QDir::separator();
            szDownload += url.fileName();
        }
        CTaskFileTransfer* task = new CTaskFileTransfer(szUrl, szDownload);
        bool check = connect(task, &CTaskFileTransfer::sigFinished,
                             this, [task, pWorker]() {
            qDebug(log) << "Download finished:" << task->Title();
            task->deleteLater();
        });
        check = connect(task, &CTaskFileTransfer::sigError,
                        this, [task, pWorker](int nErr, const QString& szError){
            qDebug(log) << "Download fail:" << nErr << szError << task->Title();
        });
        pWorker->AddTask(task);
    }
}
