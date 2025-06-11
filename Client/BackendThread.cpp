// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QMetaMethod>
#include "BackendThread.h"
#include "Backend.h"

static Q_LOGGING_CATEGORY(log, "BackendThread")

CBackendThread::CBackendThread(COperate *pOperate)
    // Note that the parent object pointer cannot be set here.
    // If set the parent, the object is also deleted
    // when the parent object (CConnecterThread) is destroyed.
    // Because it is deleted when it is finished.
    : QThread()
    , m_pOperate(pOperate)
{
    qDebug(log) << Q_FUNC_INFO;
    bool check = false;
    check = connect(this, SIGNAL(finished()),
                    this, SLOT(deleteLater()));
    Q_ASSERT(check);
}

CBackendThread::~CBackendThread()
{
    qDebug(log) << Q_FUNC_INFO;
}

/*!
 * \~chinese
 *   - 调用 COperateDesktop::InstanceBackend 实例化 CBackend
 *   - 调用 CBackend::Start() 开始
 *   - 进入事件循环
 * \~english
 *   - Call COperateDesktop::InstanceBackend to instantiate CBackend
 *   - Call CBackend::Connect() to start
 *   - Enter event loop
 */
void CBackendThread::run()
{
    qDebug(log) << Q_FUNC_INFO << "start";

    Q_ASSERT(m_pOperate);
    int nRet = 0;
    /*
    const QMetaObject* metaObject = m_pOperate->metaObject();
    QStringList methods;
    for(int i = 0; i < metaObject->methodCount(); ++i)
        methods << QString::fromLatin1(metaObject->method(i).methodSignature());
    qDebug(log) << methods;
    //*/
    if(-1 == m_pOperate->metaObject()->indexOfMethod("InstanceBackend()"))
    {
        QString szErr;
        szErr = "The class " + QString(m_pOperate->metaObject()->className())
                + " is not method InstanceBackend().";
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "BackendThread", szErr.toStdString().c_str());
    }
    CBackend* pBackend = nullptr;
    bool bRet = QMetaObject::invokeMethod(
        m_pOperate, "InstanceBackend",
        Qt::DirectConnection,
        Q_RETURN_ARG(CBackend*, pBackend));
    if(!pBackend || !bRet)
    {
        qCritical(log) << "InstanceBackend fail";
        emit m_pOperate->sigStop();
        return;
    }

    if(pBackend) {
        nRet = pBackend->Start();
        if(nRet) {
            pBackend->deleteLater();
            pBackend = nullptr;
            emit m_pOperate->sigStop();
            return;
        }
    }

    exec();

    if(pBackend) {
        pBackend->Stop();
        pBackend->deleteLater();
    }

    emit m_pOperate->sigFinished();

    qDebug(log) << Q_FUNC_INFO << "end";
}
