// Author: Kang Lin <kl222@126.com>

#include <QTimer>
#include <QLoggingCategory>

#include "Backend.h"

static Q_LOGGING_CATEGORY(log, "Operate")

CBackend::CBackend(COperate *pOperate)
    : QObject() // Because it's in a different thread with pOperate
{
    qDebug(log) << Q_FUNC_INFO;
    SetConnect(pOperate);
}

CBackend::~CBackend()
{
    qDebug(log) << Q_FUNC_INFO;
}

int CBackend::SetConnect(COperate *pOperate)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!pOperate) return -1;
    
    bool check = false;
    check = connect(this, SIGNAL(sigRunning()),
                    pOperate, SIGNAL(sigRunning()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigFinished()),
                    pOperate, SIGNAL(sigFinished()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigStop()),
                    pOperate, SIGNAL(sigStop()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigError(const int, const QString&)),
                    pOperate, SIGNAL(sigError(const int, const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigInformation(const QString&)),
                    pOperate, SIGNAL(sigInformation(const QString&)));
    Q_ASSERT(check);
    check = connect(
        this, SIGNAL(sigShowMessageBox(const QString&, const QString&,
                                 const QMessageBox::Icon&)),
        pOperate, SIGNAL(sigShowMessageBox(const QString&, const QString&,
                                 const QMessageBox::Icon&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigBlockShowMessageBox(
                              const QString&, const QString&,
                              QMessageBox::StandardButtons,
                              QMessageBox::StandardButton&,
                              bool&, QString)),
                    pOperate, SLOT(slotBlockShowMessageBox(
                        const QString&, const QString&,
                        QMessageBox::StandardButtons,
                        QMessageBox::StandardButton&,
                        bool&, QString)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigBlockInputDialog(const QString&,
                                                     const QString&,
                                                     const QString&,
                                                     QString&)),
                    pOperate, SLOT(slotBlockInputDialog(const QString&,
                                              const QString&,
                                              const QString&,
                                              QString&)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    check = connect(
        this,
        SIGNAL(sigBlockShowWidget(const QString&, int&, void*)),
        pOperate, SLOT(slotBlockShowWidget(const QString&, int&, void*)),
        Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    return 0;
}

int CBackend::Start()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = static_cast<int>(OnInit());
    if(nRet < 0) return nRet;

    if(0 == nRet)
        QTimer::singleShot(0, this, SLOT(slotTimeOut()));
    return 0;
}

int CBackend::Stop()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    nRet = OnClean();
    return nRet;
}

int CBackend::WakeUp()
{
    return 0;
}

void CBackend::slotTimeOut()
{
    //qDebug(log) << "CConnect::slotTimeOut()";
    try {
        // >= 0: continue. Call interval
        // = -1: stop
        // < -1: error
        int nTime = OnProcess();
        if(nTime >= 0)
        {
            QTimer::singleShot(nTime, this, SLOT(slotTimeOut()));
            return;
        }
        qDebug(log) << "Process fail(< -1) or stop(= -1):" << nTime;
        if(nTime < -1) {
            qCritical(log) << "Process fail:"  << nTime;
            emit sigError(nTime, "Process fail or stop");
        }
    } catch(std::exception e) {
        qCritical(log) << "Process fail:" << e.what();
        emit sigError(-2, e.what());
    }  catch (...) {
        qCritical(log) << "Process fail";
        emit sigError(-3, "Process fail");
    }

    // Error or stop, must notify user disconnect it
    emit sigStop();
}

int CBackend::OnProcess()
{
    qWarning(log) << "Need to implement CConnect::OnProcess()";
    return 0;
}
