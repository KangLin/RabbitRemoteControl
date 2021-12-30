// Author: Kang Lin <kl222@126.com>

#include "ConnecterDesktop.h"
#include <QDebug>
#include "ConnectThread.h"
#include "RabbitCommonLog.h"
#include "RabbitCommonDir.h"

CConnecterDesktop::CConnecterDesktop(CPluginViewer *parent)
    : CConnecter(parent),
      m_pThread(nullptr),
      m_pView(new CFrmViewer())
{}

CConnecterDesktop::~CConnecterDesktop()
{
    if(m_pView)
        delete m_pView;
    
    LOG_MODEL_DEBUG("CConnecterDesktop", "CConnecterDesktop::~CConnecterDesktop");
    //qDebug() << this << this->metaObject()->className();
}

QWidget *CConnecterDesktop::GetViewer()
{
    return m_pView;
}

int CConnecterDesktop::Connect()
{
    int nRet = 0;
    // Check whether the parameters are complete
    if(GetPara() && !GetPara()->GetCheckCompleted())
    {
        if(QDialog::Rejected == OpenDialogSettings())
           return -1;
        emit sigUpdateParamters(this);
    }
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
        if(GetPara() && !GetPara()->GetHost().isEmpty())
            return GetPara()->GetHost() + ":"
                   + QString::number(GetPara()->GetPort());
        else
            return CConnecter::Name();
    }
    return CConnecter::ServerName();
}

int CConnecterDesktop::Load(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(GetPara());
    Q_ASSERT(m_pView);
    if(m_pView)
    {   
        nRet = m_pView->Load(set);
        if(nRet) return nRet;
    }
    
    if(GetPara())
        nRet = GetPara()->Load(set);
        
    return nRet;
}

int CConnecterDesktop::Save(QSettings &set)
{
    int nRet = 0;
    Q_ASSERT(GetPara());
    if(m_pView)
    {
        nRet = m_pView->Save(set);
        if(nRet) return nRet;
    }
    if(GetPara())
        GetPara()->Save(set);
    return nRet;
}
