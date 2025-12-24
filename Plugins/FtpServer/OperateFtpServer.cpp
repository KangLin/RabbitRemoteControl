// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "OperateFtpServer.h"
#include "BackendFtpServer.h"
#include "DlgSettings.h"

static Q_LOGGING_CATEGORY(log, "FtpServer.Operate")
COperateFtpServer::COperateFtpServer(CPlugin *plugin)
    : COperate(plugin)
    , m_pThread(nullptr)
    , m_pView(nullptr)
    , m_pStart(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
}

COperateFtpServer::~COperateFtpServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const qint16 COperateFtpServer::Version() const
{
    return 0;
}

QWidget *COperateFtpServer::GetViewer()
{
    return m_pView;
}

int COperateFtpServer::Start()
{
    qDebug(log) << Q_FUNC_INFO;
    emit sigRunning();
    return 0;
}

int COperateFtpServer::Stop()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pStart && m_pStart->isChecked())
        m_pStart->setChecked(false);
    emit sigFinished();
    return 0;
}

CBackend* COperateFtpServer::InstanceBackend()
{
    auto p = new CBackendFtpServer(this);
    bool check = connect(p, SIGNAL(sigConnectCount(int,int,int)),
                         m_pView, SLOT(slotConnectCount(int,int,int)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigConnected(QString,quint16)),
                    m_pView, SLOT(slotConnected(QString,quint16)));
    Q_ASSERT(check);
    check = connect(p, SIGNAL(sigDisconnected(QString,quint16)),
                    m_pView, SLOT(slotDisconnected(QString,quint16)));
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigDisconnect(QString,quint16)),
                    p, SLOT(slotDisconnect(QString,quint16)));
    Q_ASSERT(check);
    return p;
}

int COperateFtpServer::SetGlobalParameters(CParameterPlugin *pPara)
{
    qDebug(log) << Q_FUNC_INFO;
    return 0;
}

QDialog *COperateFtpServer::OnOpenDialogSettings(QWidget *parent)
{
    auto pDlg = new CDlgSettings(m_Para, parent);
    return pDlg;
}

int COperateFtpServer::Load(QSettings &set)
{
    if(m_Para)
        return m_Para->Load(set);
    return 0;
}

int COperateFtpServer::Save(QSettings &set)
{
    if(m_Para)
        return m_Para->Save(set);
    return 0;
}

int COperateFtpServer::Initial()
{
    qDebug(log) << Q_FUNC_INFO;
    int nRet = 0;

    m_Para = QSharedPointer<CParameterFtpServer>(new CParameterFtpServer());
    if(!m_Para)
        return -1;

    nRet = COperate::Initial();
    if(nRet)
        return nRet;

    m_pStart = m_Menu.addAction(QIcon::fromTheme("media-playback-start"), tr("Start server"));
    bool check = connect(m_pStart, &QAction::toggled, this, &COperateFtpServer::slotStart);
    Q_ASSERT(check);
    m_pStart->setCheckable(true);
    m_pStart->setToolTip(m_pStart->text());
    if(m_pActionSettings) {
        m_Menu.addSeparator();
        m_Menu.addAction(m_pActionSettings);
    }

    m_pView = new CFrmMain(this);
    return nRet;
}

int COperateFtpServer::Clean()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pView)
        delete m_pView;
    return 0;
}

QSharedPointer<CParameterFtpServer> COperateFtpServer::GetParameter()
{
    return m_Para;
}

void COperateFtpServer::slotStart(bool checked)
{
    qDebug(log) << Q_FUNC_INFO << m_pStart->isChecked();
    if(!checked) {
        m_pStart->setText(tr("Start server"));
        m_pStart->setToolTip(m_pStart->text());
        m_pStart->setIcon(QIcon::fromTheme("media-playback-start"));
        if(m_pThread)
        {
            m_pThread->quit();
            //Don't delete m_pThread, See CBackendThread
            m_pThread = nullptr;
        }
        return;
    }

    m_pStart->setText(tr("Stop server"));
    m_pStart->setToolTip(m_pStart->text());
    m_pStart->setIcon(QIcon::fromTheme("media-playback-stop"));
    Q_ASSERT(!m_pThread);
    m_pThread = new CBackendThread(this, false);
    if(!m_pThread) {
        qCritical(log) << "new CBackendThread fail";
        return;
    }
    
    m_pThread->start();
}
