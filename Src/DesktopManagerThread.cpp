#include <QLoggingCategory>
#include "DesktopManagerThread.h"
#include "DesktopManager.h"
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    #include "Unix/DesktopManagerUnix.h"
#endif

static Q_LOGGING_CATEGORY(log, "Plugin.Hook.DesktopManager.Thread")
CDesktopManagerThread::CDesktopManagerThread(CParameterPlugin *pPara, QObject *parent)
    : QThread{parent}
    , m_pDesktopManager(nullptr)
    , m_pPara(pPara)
{
    qDebug(log) << Q_FUNC_INFO;
}

CDesktopManagerThread::~CDesktopManagerThread()
{
    qDebug(log) << Q_FUNC_INFO;
}

bool CDesktopManagerThread::IsStart()
{
    return m_Start != 0;
}

void CDesktopManagerThread::run()
{
    qDebug(log) << "CDesktopManagerThread::run()";
    CDesktopManager* pMgr = nullptr;
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    pMgr = new CDesktopManagerUnix(m_pPara);
#endif
    if(pMgr) {
        bool check = connect(this, &CDesktopManagerThread::sigDisableShortcuts,
                        pMgr, &CDesktopManager::slotDisableShortcuts);
        Q_ASSERT(check);
        check = connect(this, &CDesktopManagerThread::sigRestoreShortcuts,
                        pMgr, &CDesktopManager::slotRestoreShortcuts);
        Q_ASSERT(check);
    }

    m_Start = 1;

    exec();

    if(pMgr)
        pMgr->deleteLater();

    qDebug(log) << "CDesktopManagerThread::run() end";
}
