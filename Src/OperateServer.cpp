// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>

#include "BackendThread.h"
#include "FrmViewServer.h"
#include "OperateServer.h"

static Q_LOGGING_CATEGORY(log, "Operate.Server")
COperateServer::COperateServer(CPlugin *plugin)
    : COperate(plugin)
    , m_pViewer(nullptr)
    , m_pThread(nullptr)
    , m_pActionStart(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateServer::~COperateServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

QWidget *COperateServer::GetViewer()
{
    return m_pViewer;
}

int COperateServer::Start()
{
    qDebug(log) << Q_FUNC_INFO;
    emit sigRunning();
    return 0;
}

int COperateServer::Stop()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;
    if(m_pActionStart && m_pActionStart->isChecked()) {
        bool check = connect(m_pThread, SIGNAL(finished()),
                             this, SIGNAL(sigFinished()));
        Q_ASSERT(check);
        m_pActionStart->setChecked(false);
    } else
        emit sigFinished();

    return nRet;
}

int COperateServer::Initial()
{
    int nRet = 0;
    nRet = COperate::Initial();
    if(nRet) return nRet;
    m_pViewer = new CFrmViewServer();

    m_pActionStart = m_Menu.addAction(
        QIcon::fromTheme("media-playback-start"), tr("Start server"));
    if(m_pActionStart) {
        m_pActionStart->setStatusTip(m_pActionStart->text());
        m_pActionStart->setToolTip(m_pActionStart->text());
        m_pActionStart->setCheckable(true);
        bool check = connect(m_pActionStart, &QAction::toggled,
                             this, &COperateServer::slotStart);
        Q_ASSERT(check);
        m_pViewer->AddActionInToolBar(m_pActionStart);
    }
    m_Menu.addSeparator();
    if(m_pActionSettings) {
        m_Menu.addAction(m_pActionSettings);
        m_pViewer->AddActionInToolBar(m_pActionSettings);
    }
    return nRet;
}

int COperateServer::Clean()
{
    int nRet = 0;
    if(m_pViewer) {
        delete m_pViewer;
        m_pViewer = nullptr;
    }
    nRet = COperate::Clean();
    return nRet;
}

void COperateServer::slotStart(bool checked)
{
    qDebug(log) << Q_FUNC_INFO << m_pActionStart->isChecked();
    bool check = false;
    if(!checked) {
        m_pActionStart->setText(tr("Start server"));
        m_pActionStart->setToolTip(m_pActionStart->text());
        m_pActionStart->setIcon(QIcon::fromTheme("media-playback-start"));
        if(m_pThread)
        {
            slotSetSecurityLevel(CSecurityLevel::Level::No);
            m_pThread->quit();
            // NOTE: Don't delete m_pThread, See CBackendThread
            m_pThread = nullptr;
        }
        return;
    }

    m_pActionStart->setText(tr("Stop server"));
    m_pActionStart->setToolTip(m_pActionStart->text());
    m_pActionStart->setIcon(QIcon::fromTheme("media-playback-stop"));
    Q_ASSERT(!m_pThread);
    m_pThread = new CBackendThread(this, false);
    if(!m_pThread) {
        qCritical(log) << "new CBackendThread fail";
        return;
    }
    check = connect(this, &COperateServer::sigError,
                    this, [this](const int nError, const QString &szError){
                        m_pActionStart->setChecked(false);
                    });
    Q_ASSERT(check);
    m_pThread->start();
}
