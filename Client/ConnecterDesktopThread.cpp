// Author: Kang Lin <kl222@126.com>

#include "ConnecterDesktopThread.h"
#include <QDebug>
#include "ConnectThread.h"
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "Client.Connecter.DesktopThread")

CConnecterDesktopThread::CConnecterDesktopThread(CPluginClient *parent)
    : CConnecter(parent),
      m_pThread(nullptr),
      m_pView(new CFrmViewer())
{}

CConnecterDesktopThread::~CConnecterDesktopThread()
{
    if(m_pView)
    {
        delete m_pView;
        m_pView = nullptr;
    }
    
    qDebug(log) << "CConnecterDesktopThread::~CConnecterDesktopThread()";
    //qDebug(log) << this << this->metaObject()->className();
}

QWidget *CConnecterDesktopThread::GetViewer()
{
    return m_pView;
}

CParameterBase *CConnecterDesktopThread::GetParameter()
{
    return CConnecter::GetParameter();
}

int CConnecterDesktopThread::Connect()
{
    qDebug(log) << "CConnecterDesktopThread::Connect()";
    int nRet = 0;
    m_pThread = new CConnectThread(this);
    if(!m_pThread) {
        qCritical(log) << "new CConnectThread fail";
        return -2;
    }
    
    m_pThread->start();
    
    return nRet;
}

int CConnecterDesktopThread::DisConnect()
{
    qDebug(log) << "CConnecterDesktopThread::DisConnect()";
    int nRet = 0;
    if(m_pThread)
    {
        m_pThread->quit();
        //Don't delete m_pThread, See CConnectThread
        m_pThread = nullptr;
    }
    return nRet;
}

QString CConnecterDesktopThread::ServerName()
{
    if(GetParameter())
        if(!GetParameter()->GetShowServerName()
                || CConnecter::ServerName().isEmpty())
        {
            if(!GetParameter()->m_Net.GetHost().isEmpty())
                return GetParameter()->m_Net.GetHost() + ":"
                        + QString::number(GetParameter()->m_Net.GetPort());
        }
    return CConnecter::ServerName();
}

int CConnecterDesktopThread::Load(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(GetParameter());
    if(GetParameter())
        nRet = GetParameter()->Load(set);
    Q_ASSERT(m_pView);
    if(m_pView)
    {
        m_pView->slotSetAdaptWindows(GetParameter()->GetAdaptWindows());
        m_pView->slotSetZoomFactor(GetParameter()->GetZoomFactor());
    }
    return nRet;
}

int CConnecterDesktopThread::Save(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(GetParameter());
    if(GetParameter()) {
        if(m_pView)
        {
            GetParameter()->SetAdaptWindows(m_pView->GetAdaptWindows());
            GetParameter()->SetZoomFactor(m_pView->GetZoomFactor());
        }
        GetParameter()->Save(set);
    }
    return nRet;
}
