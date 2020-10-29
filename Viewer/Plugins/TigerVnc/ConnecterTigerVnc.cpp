#include "ConnecterTigerVnc.h"
#include "rfb/encodings.h"
#include <QDebug>

CConnecterTigerVnc::CConnecterTigerVnc(QObject *parent)
    : CConnecter(parent),
      m_pThread(nullptr)
{
    //TODO: delete it
    m_Para.szServerName = "fmpixel.f3322.net:5906";
    m_Para.szPassword = "yly075077";
    
    m_Para.bShared = true;
    m_Para.bBufferEndRefresh = true;
    m_Para.bAutoSelect = true;
    m_Para.nColorLevel = CConnectTigerVnc::Full;
    m_Para.nEncoding = rfb::encodingTight;
    m_Para.bCompressLevel = true;
    m_Para.nCompressLevel = 2;
    m_Para.bNoJpeg = false;
    m_Para.nQualityLevel = 8;

    m_pView = new CFrmViewer();
}

CConnecterTigerVnc::~CConnecterTigerVnc()
{
    qDebug() << "CConnecterTigerVnc::~CConnecterTigerVnc()";
    if(m_pThread)
    {
        m_pThread->wait();
        delete m_pThread;
    }
}

CFrmViewer* CConnecterTigerVnc::GetViewer()
{
    return m_pView;
}

QDialog *CConnecterTigerVnc::GetDialogSettings(QWidget *parent)
{
    CDlgSettings* p = new CDlgSettings(&m_Para);
    p->setAttribute(Qt::WA_DeleteOnClose);
    return p;
}

int CConnecterTigerVnc::Connect()
{
    if(nullptr == m_pThread)
    {
        m_pThread = new CConnectThread(m_pView, this);
    }
    m_pThread->start();
    return 0;
}

int CConnecterTigerVnc::DisConnect()
{
    if(!m_pThread) return -1;
    
    m_pThread->m_bExit = true;
    emit sigDisconnected();
    return 0;
}
