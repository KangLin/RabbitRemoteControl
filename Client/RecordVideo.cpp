#include "RecordVideo.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Record.Video")
static Q_LOGGING_CATEGORY(logThread, "Record.Video.Thread")

CRecordVideoThread::CRecordVideoThread()
    /*! \note that the parent object pointer cannot be set here.
     * If set the parent, the object is also deleted
     * when the parent object (CConnecterThread) is destroyed.
     * Because it is deleted when it is finished.
     */
    : QThread()
{
    bool check = connect(this, SIGNAL(finished()),
                         this, SLOT(deleteLater()));
    Q_ASSERT(check);
}

int CRecordVideoThread::SetFile(const QString &szFile)
{
    m_szFile = szFile;
    return 0;
}

void CRecordVideoThread::run()
{
    bool check = false;
    qDebug(logThread) << "Record video thread start ...";
    CRecordVideo record;
    int nRet = record.Start(m_szFile);
    if(nRet) {
        qCritical(logThread) << "Start record fail:" << nRet;
        emit sigStatusChanged(CFrmViewer::RecordVideoStatus::Error);
        return;
    }
    check = connect(&record, SIGNAL(sigStatusChanged(CFrmViewer::RecordVideoStatus)),
                    this, SIGNAL(sigStatusChanged(CFrmViewer::RecordVideoStatus)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdate(QImage)),
                    &record, SLOT(slotUpdate(QImage)));
    Q_ASSERT(check);
    check = connect(&record, SIGNAL(sigError(int,QString)),
                    this, SIGNAL(sigError(int,QString)));
    Q_ASSERT(check);
    emit sigStatusChanged(CFrmViewer::RecordVideoStatus::Recording);

    exec();

    nRet = record.Stop();
    if(nRet) {
        qCritical(logThread) << "Stop record fail:" << nRet;
        emit sigStatusChanged(CFrmViewer::RecordVideoStatus::Error);
    }
    else
        emit sigStatusChanged(CFrmViewer::RecordVideoStatus::Stop);
    qDebug(logThread) << "Record video thread end";
}

CRecordVideo::CRecordVideo(QObject *parent)
    : QObject{parent}
{
}

int CRecordVideo::Start(const QString &szFile)
{
    int nRet = 0;
    qDebug(log) << "Start";
    //TODO: Initial video parameters and resource

    return nRet;
}

int CRecordVideo::Stop()
{
    int nRet = 0;
    qDebug(log) << "Stop";
    //TODO: Clean resource

    return nRet;
}

void CRecordVideo::slotUpdate(QImage img)
{
    qDebug(log) << "Update image";
    //TODO: save video to file
    QThread::sleep(3);
    emit sigError(3, "record video error");
}
