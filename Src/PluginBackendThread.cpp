// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QMetaMethod>
#include "PluginThread.h"
#include "PluginBackendThread.h"

static Q_LOGGING_CATEGORY(log, "Plugin.Backend.Thread")
CPluginBackendThread::CPluginBackendThread(QObject *parent)
    : CPlugin{parent}
    , m_pThread(nullptr)
{
    m_pThread = new CPluginThread(); // Note that the parent object pointer cannot be set here.
    if(m_pThread)
    {
        // The object is also deleted when the thread finishes executing.
        bool check = connect(m_pThread, SIGNAL(finished()),
                             m_pThread, SLOT(deleteLater()));
        Q_ASSERT(check);
        m_pThread->start();
    }
}

CPluginBackendThread::~CPluginBackendThread()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pThread)
        m_pThread->quit(); // The don't deleteLater().
    // because of it is connected finished signal
}

/*!
 * \brief Create operate and connect signals to CPluginThread
 * \see CPluginThread
 */
COperate* CPluginBackendThread::CreateOperate(const QString &szId, CParameterPlugin *para)
{
    auto* pOperate = CPlugin::CreateOperate(szId, para);
    if(!pOperate) return nullptr;
    QString szMsg;
    szMsg = "Please generate plugin from template using the script `./Script/create_plugin.sh --template=QtEvent`. or ";
    szMsg += QString(pOperate->metaObject()->className());
    szMsg += " include `sigNewBackend(COperate*)`, `sigDeleteBackend(COperate*)` and `Q_INVOKABLE virtual CBackend* InstanceBackend()`";
    /*
    const QMetaObject* metaObject = pOperate->metaObject();
    QStringList methods;
    for(int i = 0; i < metaObject->methodCount(); ++i)
        methods << QString::fromLatin1(metaObject->method(i).methodSignature());
    qDebug(log) << methods;
    //*/
    if(-1 == pOperate->metaObject()->indexOfMethod("InstanceBackend()")) {
        QString szErr;
        szErr = "The class " + QString(pOperate->metaObject()->className())
                + " has not method InstanceBackend(). "
                  "Please add 'Q_INVOKABLE virtual CBackend* InstanceBackend()' to "
                + QString(pOperate->metaObject()->className());
        szErr += ". " + szMsg;
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "CPluginBackendThread", szErr.toStdString().c_str());
    }
    if(-1 == pOperate->metaObject()->indexOfMethod("sigNewBackend(COperate*)")) {
        QString szErr;
        szErr = "The class " + QString(pOperate->metaObject()->className())
                + " has not signal sigNewBackend(). "
                  "Please add 'void sigNewBackend();' to "
                + QString(pOperate->metaObject()->className());
        szErr += ". " + szMsg;
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "CPluginBackendThread", szErr.toStdString().c_str());
    }
    if(-1 == pOperate->metaObject()->indexOfMethod("sigDeleteBackend(COperate*)")) {
        QString szErr;
        szErr = "The class " + QString(pOperate->metaObject()->className())
                + " has not signal sigDeleteBackend(). "
                  "Please add 'void sigDeleteBackend();' to "
                + QString(pOperate->metaObject()->className());
        szErr += ". " + szMsg;
        qCritical(log) << szErr;
        Q_ASSERT_X(false, "CPluginBackendThread", szErr.toStdString().c_str());
    }

    Q_ASSERT(m_pThread);

    bool check = connect(pOperate, SIGNAL(sigNewBackend(COperate*)),
                         m_pThread, SIGNAL(sigNewBackend(COperate*)));
    Q_ASSERT(check);
    check = connect(pOperate, SIGNAL(sigDeleteBackend(COperate*)),
                    m_pThread, SIGNAL(sigDeleteBackend(COperate*)));
    Q_ASSERT(check);

    return pOperate;
}