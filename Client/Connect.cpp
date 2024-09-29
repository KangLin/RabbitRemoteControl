// Author: Kang Lin <kl222@126.com>
#include <QTimer>
#include <QLoggingCategory>

#include "Connect.h"

static Q_LOGGING_CATEGORY(log, "Client.Connect")

CConnect::CConnect(CConnecter *pConnecter) : QObject()
{
    SetConnecter(pConnecter);
}

CConnect::~CConnect()
{
    qDebug(log) << "CConnect::~CConnect()";
}

int CConnect::SetConnecter(CConnecter* pConnecter)
{
    qDebug(log) << "CConnect::SetConnecter" << pConnecter;
    if(!pConnecter) return -1;
    
    bool check = false;
    check = connect(this, SIGNAL(sigConnected()),
                    pConnecter, SIGNAL(sigConnected()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnect()),
                    pConnecter, SIGNAL(sigDisconnect()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnected()),
                    pConnecter, SIGNAL(sigDisconnected()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigError(const int, const QString&)),
                    pConnecter, SIGNAL(sigError(const int, const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigInformation(const QString&)),
                    pConnecter, SIGNAL(sigInformation(const QString&)));
    Q_ASSERT(check);
    check = connect(
        this, SIGNAL(sigShowMessageBox(const QString&, const QString&,
                                 const QMessageBox::Icon&)),
        pConnecter, SIGNAL(sigShowMessageBox(const QString&, const QString&,
                                 const QMessageBox::Icon&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigBlockShowMessageBox(
                              const QString&, const QString&,
                              QMessageBox::StandardButtons,
                              QMessageBox::StandardButton&,
                              bool&, QString)),
                    pConnecter, SLOT(slotBlockShowMessageBox(
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
                    pConnecter, SLOT(slotBlockInputDialog(const QString&,
                                                          const QString&,
                                                          const QString&,
                                                          QString&)),
                    Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    check = connect(
        this,
        SIGNAL(sigBlockShowWidget(const QString&, int&, void*)),
        pConnecter, SLOT(slotBlockShowWidget(const QString&, int&, void*)),
        Qt::BlockingQueuedConnection);
    Q_ASSERT(check);
    return 0;
}

int CConnect::Connect()
{
    qDebug(log) << "CConnect::Connect()";
    int nRet = 0;
    nRet = static_cast<int>(OnInit());
    if(nRet < 0) return nRet;

    if(0 == nRet)
        QTimer::singleShot(0, this, SLOT(slotTimeOut()));
    return 0;
}

int CConnect::Disconnect()
{
    qDebug(log) << "CConnect::Disconnect()";
    int nRet = 0;
    nRet = OnClean();
    return nRet;
}

void CConnect::slotTimeOut()
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
    emit sigDisconnect();
}

int CConnect::OnProcess()
{
    qWarning(log) << "Need to implement CConnect::OnProcess()";
    return 0;
}

