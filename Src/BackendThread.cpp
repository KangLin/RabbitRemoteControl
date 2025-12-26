// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QMetaMethod>
#include "BackendThread.h"
#include "Backend.h"

static Q_LOGGING_CATEGORY(log, "BackendThread")

CBackendThread::CBackendThread(COperate *pOperate, bool bFinishedSignal)
    // Note that the parent object pointer cannot be set here.
    // If set the parent, the object is also deleted
    // when the parent object (CConnecterThread) is destroyed.
    // Because it is deleted when it is finished.
    : QThread()
    , m_pOperate(pOperate)
    , m_pBackend(nullptr)
    , m_bFinishedSignal(bFinishedSignal)
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

void CBackendThread::quit()
{
    qDebug(log) << Q_FUNC_INFO;
    QThread::quit();
    if(m_pBackend) {
        m_pBackend->WakeUp();
    }
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
                + " has not method InstanceBackend()."
                  "Please add 'Q_INVOKABLE virtual CBackend* InstanceBackend()' to "
                + QString(m_pOperate->metaObject()->className());
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "BackendThread", szErr.toStdString().c_str());
    }

    bool bRet = QMetaObject::invokeMethod(
        m_pOperate, "InstanceBackend",
        Qt::DirectConnection,
        Q_RETURN_ARG(CBackend*, m_pBackend));
    if(!m_pBackend || !bRet)
    {
        qCritical(log) << "InstanceBackend fail";
        if(m_bFinishedSignal) {
            emit m_pOperate->sigStop();
            emit m_pOperate->sigFinished();
        }
        return;
    }

    if(m_pBackend) {
        nRet = m_pBackend->Start();
        if(nRet) {
            qCritical(log) << "Backend start fail" << nRet;
            m_pBackend->Stop();
            m_pBackend->deleteLater();
            m_pBackend = nullptr;
            if(m_bFinishedSignal) {
                emit m_pOperate->sigStop();
                emit m_pOperate->sigFinished();
            }
            return;
        }
    }

    exec();

    if(m_pBackend) {
        m_pBackend->Stop();
        m_pBackend->deleteLater();
    }

    if(m_bFinishedSignal)
        emit m_pOperate->sigFinished();

    qDebug(log) << Q_FUNC_INFO << "end";
}
