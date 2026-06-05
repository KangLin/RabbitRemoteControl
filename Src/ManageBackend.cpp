// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ManageBackend.h"

static Q_LOGGING_CATEGORY(log, "Manage.Backend")
CManageBackend::CManageBackend(QObject *parent)
    : QObject{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CManageBackend::~CManageBackend()
{
    qDebug(log) << Q_FUNC_INFO;
    foreach(auto pBackend, m_Backends)
    {
        pBackend->Stop();
        pBackend->deleteLater();
    }
}

void CManageBackend::slotNewBackend(COperate *pOperate)
{
    qDebug(log) << Q_FUNC_INFO;
    if(-1 == pOperate->metaObject()->indexOfMethod("InstanceBackend()"))
    {
        QString szErr;
        szErr = "The class " + QString(pOperate->metaObject()->className())
                + " has not method InstanceBackend()."
                  "Please add 'Q_INVOKABLE virtual CBackend* InstanceBackend()' to "
                + QString(pOperate->metaObject()->className());
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "BackendThread", szErr.toStdString().c_str());
    }

    CBackend* pBackend = nullptr;
    bool bRet = QMetaObject::invokeMethod(
        pOperate, "InstanceBackend",
        Qt::DirectConnection,
        Q_RETURN_ARG(CBackend*, pBackend));
    if(!pBackend || !bRet)
    {
        qCritical(log) << "InstanceBackend fail";
        emit pOperate->sigStop();
        return;
    }
    m_Backends.insert(pOperate, pBackend);
    int nRet = pBackend->Start();
    if(nRet)
        emit pOperate->sigStop();
}

void CManageBackend::slotDeleteBackend(COperate *pOperate)
{
    qDebug(log) << Q_FUNC_INFO;
    auto it = m_Backends.find(pOperate);
    if(m_Backends.end() == it) return;
    auto pBackend = it.value();
    m_Backends.remove(pOperate);
    pBackend->Stop();
    pBackend->deleteLater();
    //emit pOperate->sigFinished();
}