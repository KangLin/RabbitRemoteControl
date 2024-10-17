#include "RecordVideo.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "RecordVideo")
CRecordVideo::CRecordVideo(QObject *parent)
    : QObject{parent}
{
    bool check = connect(this, SIGNAL(sigUpdate(QImage)),
                         this, SLOT(slotUpdate(QImage)));
    Q_ASSERT(check);
}

int CRecordVideo::Start(const QString &szFile)
{
    int nRet = 0;
    qDebug(log) << "Start";

    return nRet;
}

int CRecordVideo::Stop()
{
    int nRet = 0;
    qDebug(log) << "Stop";

    return nRet;
}

void CRecordVideo::slotUpdate(QImage img)
{
    qDebug(log) << "Update image";
}
