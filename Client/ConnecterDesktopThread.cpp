// Author: Kang Lin <kl222@126.com>

#include "ConnecterDesktopThread.h"
#include <QDebug>
#include "ConnectThread.h"
#include "RabbitCommonDir.h"
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(Client)

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
    
    qDebug(Client) << "CConnecterDesktopThread::~CConnecterDesktopThread";
    //qDebug() << this << this->metaObject()->className();
}

QWidget *CConnecterDesktopThread::GetViewer()
{
    return m_pView;
}

CParameterConnecter* CConnecterDesktopThread::GetParameter()
{
    return CConnecter::GetParameter();
}

int CConnecterDesktopThread::Connect()
{
    int nRet = 0;
    m_pThread = new CConnectThread(this);
    if(!m_pThread)
        return -2;
    bool check = false;
    check = connect(m_pThread, SIGNAL(finished()),
                    m_pThread, SLOT(deleteLater()));
    Q_ASSERT(check);

    m_pThread->start();
    
    return nRet;
}

int CConnecterDesktopThread::DisConnect()
{
    int nRet = 0;
    if(m_pThread)
    {
        m_pThread->quit();
        //Don't delete m_pThread, See CConnecterDesktopThread::Connect()
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
            if(!GetParameter()->GetHost().isEmpty())
                return GetParameter()->GetHost() + ":"
                        + QString::number(GetParameter()->GetPort());
        }
    return CConnecter::ServerName();
}

int CConnecterDesktopThread::Load(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(GetParameter());
    Q_ASSERT(m_pView);
    if(m_pView)
    {   
        nRet = m_pView->Load(set);
        if(nRet) return nRet;
    }

    if(GetParameter())
        nRet = GetParameter()->Load(set);
        
    return nRet;
}

int CConnecterDesktopThread::Save(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(GetParameter());
    if(m_pView)
    {
        nRet = m_pView->Save(set);
        if(nRet) return nRet;
    }
    if(GetParameter())
        GetParameter()->Save(set);
    return nRet;
}

