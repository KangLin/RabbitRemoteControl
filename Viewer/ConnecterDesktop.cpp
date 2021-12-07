// Author: Kang Lin <kl222@126.com>

#include "ConnecterDesktop.h"
#include <QDebug>
#include "ConnectThread.h"
#include "RabbitCommonLog.h"

CConnecterDesktop::CConnecterDesktop(CPluginViewer *parent)
    : CConnecter(parent),
      m_pThread(nullptr),
      m_pView(new CFrmViewer()),
      m_pParameter(nullptr)
{}

CConnecterDesktop::~CConnecterDesktop()
{
    if(m_pView)
        delete m_pView;
    
    LOG_MODEL_DEBUG("CConnecterDesktop", "CConnecterDesktop::~CConnecterDesktop");
    //qDebug() << this << this->metaObject()->className();
}

const QString CConnecterDesktop::Name()
{
    if(m_pParameter && !m_pParameter->GetName().isEmpty())
        return m_pParameter->GetName();
    return ServerName();
}

QWidget *CConnecterDesktop::GetViewer()
{
    return m_pView;
}

int CConnecterDesktop::Connect()
{
    int nRet = 0;
    m_pThread = new CConnectThread(this);
    if(!m_pThread)
        return -1;
    bool check = false;
    check = connect(m_pThread, SIGNAL(finished()),
                    m_pThread, SLOT(deleteLater()));
    Q_ASSERT(check);

    m_pThread->start();
    
    return nRet;
}

int CConnecterDesktop::DisConnect()
{
    int nRet = 0;
    if(m_pThread)
    {
        m_pThread->quit();
        //Don't delete m_pThread, See CConnecterDesktop::Connect()
        m_pThread = nullptr;
    }
    return nRet;
}

QString CConnecterDesktop::ServerName()
{
    if(CConnecter::ServerName().isEmpty())
    {
        if(m_pParameter && !m_pParameter->GetHost().isEmpty())
            return m_pParameter->GetHost() + ":"
                   + QString::number(m_pParameter->GetPort());
        else
            return CConnecter::Name();
    }
    return CConnecter::ServerName();
}

int CConnecterDesktop::Load(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(m_pParameter);
    
    nRet = OnLoad(set);
    return nRet;
}

int CConnecterDesktop::Save(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(m_pParameter);
   
    nRet = OnSave(set);
    return nRet;
}

int CConnecterDesktop::OnLoad(QSettings &set)
{
    Q_UNUSED(set);
    return 0;
}

int CConnecterDesktop::OnSave(QSettings &set)
{
    Q_UNUSED(set);
    return 0;
}
