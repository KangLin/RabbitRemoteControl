//! @author: Kang Lin(kl222@126.com)

#include "ConnecterPlugins.h"
#include <QDebug>
#include "ConnectThread.h"
#include "RabbitCommonLog.h"

CConnecterPlugins::CConnecterPlugins(CPluginFactory *parent)
    : CConnecter(parent),
      m_bExit(false),
      m_pThread(nullptr),
      m_pView(new CFrmViewer()),
      m_pParamter(nullptr)
{}

CConnecterPlugins::~CConnecterPlugins()
{
    if(m_pThread)
    {
        m_pThread->wait();
        delete m_pThread;
    }

    if(m_pView)
        delete m_pView;
    
    qDebug() << this << this->metaObject()->className();
}

QWidget *CConnecterPlugins::GetViewer()
{
    return m_pView;
}

int CConnecterPlugins::OnRun()
{
    //LOG_MODEL_DEBUG("CConnecterBackThread", "Current thread: 0x%X", QThread::currentThreadId());
    int nRet = -1;
    CConnect* pConnect = InstanceConnect();
    
    do{
        CConnect* pConnect = InstanceConnect();
        if(nullptr == pConnect) break;
        
        nRet = pConnect->Initialize();
        if(nRet) break;
        
        nRet = pConnect->Connect();
        if(nRet) break;
        
        while (!m_bExit) {
            try {
                // 0 : continue
                // 1: exit
                // < 0: error
                nRet = pConnect->Process();
                if(nRet) break;
            }  catch (...) {
                LOG_MODEL_ERROR("ConnecterBackThread", "process fail:%d", nRet);
                break;
            }
        }
        
    }while (0);

    emit sigDisconnected();

    pConnect->Clean();
    delete pConnect;
    qDebug() << "CConnecterPlugins::OnRun() end";
    return nRet;
}

int CConnecterPlugins::Connect()
{
    int nRet = 0;
    m_pThread = new CConnectThread(this);
    if(m_pThread)
        m_pThread->start();
    
    nRet = OnConnect();
    return nRet;
}

int CConnecterPlugins::DisConnect()
{
    int nRet = 0;
    m_bExit = true;
    nRet = OnDisConnect();
    
    return nRet;
}

int CConnecterPlugins::OnConnect()
{
    return 0;
}

int CConnecterPlugins::OnDisConnect()
{
    emit sigDisconnected();
    return 0;
}

QString CConnecterPlugins::GetServerName()
{
    if(m_szServerName.isEmpty() && m_pParamter)
        m_szServerName = m_pParamter->szHost + ":" + QString::number(m_pParamter->nPort);
    return CConnecter::GetServerName();
}


int CConnecterPlugins::Load(QDataStream &d)
{
    int nRet = 0;
    Q_ASSERT(m_pParamter);
    qint16 version = 0;
    d >> version;
    d >> m_pParamter->szHost
      >> m_pParamter->nPort
      >> m_pParamter->szUser
      >> m_pParamter->bSavePassword;
    
    if(m_pParamter->bSavePassword)
        d >> m_pParamter->szPassword;
    
    d >> m_pParamter->bOnlyView
      >> m_pParamter->bLocalCursor
      >> m_pParamter->bClipboard;
    
    nRet = OnLoad(d);
    return nRet;
}

int CConnecterPlugins::Save(QDataStream &d)
{
    int nRet = 0;
    Q_ASSERT(m_pParamter);
    d << Version()
      << m_pParamter->szHost
      << m_pParamter->nPort
      << m_pParamter->szUser
      << m_pParamter->bSavePassword;
    
    if(m_pParamter->bSavePassword)
        d << m_pParamter->szPassword;
    
    d << m_pParamter->bOnlyView
      << m_pParamter->bLocalCursor
      << m_pParamter->bClipboard;
    
    nRet = OnSave(d);
    return nRet;
}

int CConnecterPlugins::OnLoad(QDataStream& d)
{
    Q_UNUSED(d);
    return 0;
}

int CConnecterPlugins::OnSave(QDataStream& d)
{
    Q_UNUSED(d);
    return 0;
}
