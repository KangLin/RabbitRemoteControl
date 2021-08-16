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
    if(m_pParameter && !m_pParameter->szName.isEmpty())
        return m_pParameter->szName;
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
                    this, SIGNAL(sigDisconnected()));
    Q_ASSERT(check);
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
        m_pThread->quit();
    return nRet;
}

QString CConnecterDesktop::ServerName()
{
    if(CConnecter::ServerName().isEmpty())
    {
        if(m_pParameter && !m_pParameter->szHost.isEmpty())
            return m_pParameter->szHost + ":"
                   + QString::number(m_pParameter->nPort);
        else
            return CConnecter::Name();
    }
    return CConnecter::ServerName();
}

int CConnecterDesktop::Load(QDataStream &d)
{
    int nRet = 0;
    Q_ASSERT(m_pParameter);
    qint16 version = 0;
    d >> version 
      >> *m_pParameter;

    nRet = OnLoad(d);
    return nRet;
}

int CConnecterDesktop::Save(QDataStream &d)
{
    int nRet = 0;
    Q_ASSERT(m_pParameter);
    d << Version()
      << *m_pParameter;
    
    nRet = OnSave(d);
    return nRet;
}

int CConnecterDesktop::OnLoad(QDataStream& d)
{
    Q_UNUSED(d);
    return 0;
}

int CConnecterDesktop::OnSave(QDataStream& d)
{
    Q_UNUSED(d);
    return 0;
}
