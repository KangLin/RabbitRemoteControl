#include "ConnecterTigerVnc.h"
#include "rfb/encodings.h"
#include <QDebug>

CConnecterTigerVnc::CConnecterTigerVnc(QObject *parent)
    : CConnecter(parent),
      m_pThread(nullptr)
{
    m_Para.bSave = true;
    m_Para.bShared = true;
    m_Para.bBufferEndRefresh = true;
    m_Para.bLocalCursor = true;
    m_Para.bClipboard = true;
    m_Para.bSupportsDesktopResize = true;
    
    m_Para.bAutoSelect = true;
    m_Para.nColorLevel = CConnectTigerVnc::Full;
    m_Para.nEncoding = rfb::encodingTight;
    m_Para.bCompressLevel = true;
    m_Para.nCompressLevel = 2;
    m_Para.bNoJpeg = false;
    m_Para.nQualityLevel = 8;
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

QString CConnecterTigerVnc::Name()
{
    QString szName = m_Para.szServerName;
    szName.replace(":", "_");
    return szName;
}

QString CConnecterTigerVnc::Description()
{
    return Protol() + ":" + m_Para.szServerName;
}

QString CConnecterTigerVnc::Protol()
{
    return "VNC";
}

QDialog *CConnecterTigerVnc::GetDialogSettings(QWidget *parent)
{
    Q_UNUSED(parent)
    CDlgSettings* p = new CDlgSettings(&m_Para);
    p->setAttribute(Qt::WA_DeleteOnClose);
    return p;
}

int CConnecterTigerVnc::Connect()
{
    if(nullptr == m_pThread)
    {
        m_pThread = new CConnectThread(GetViewer(), this);
    }
    m_pThread->start();
    return 0;
}

int CConnecterTigerVnc::DisConnect()
{
    if(!m_pThread) return -1;
    
    m_pThread->m_bExit = true;
    // Actively disconnect, without waiting for the thread to exit and then disconnect
    emit sigDisconnected();
    return 0;
}

int CConnecterTigerVnc::Save(QDataStream & d)
{
    d << m_Para.szServerName
      << m_Para.szUser
      << m_Para.szPassword
      << m_Para.bSave
      << m_Para.bShared
      << m_Para.bBufferEndRefresh
      << m_Para.bLocalCursor
      << m_Para.bSupportsDesktopResize
      << m_Para.bClipboard
      << m_Para.bAutoSelect
      << static_cast<int>(m_Para.nColorLevel)
      << m_Para.nEncoding
      << m_Para.bCompressLevel
      << m_Para.nCompressLevel
      << m_Para.bNoJpeg
      << m_Para.nQualityLevel
         ;
    return 0;
}

int CConnecterTigerVnc::Load(QDataStream &d)
{
    int nColorLevel = CConnectTigerVnc::Full;
    d >> m_Para.szServerName
           >> m_Para.szUser
           >> m_Para.szPassword
           >> m_Para.bSave
           >> m_Para.bShared
           >> m_Para.bBufferEndRefresh
           >> m_Para.bLocalCursor
           >> m_Para.bSupportsDesktopResize
           >> m_Para.bClipboard
           >> m_Para.bAutoSelect
           >> nColorLevel
           >> m_Para.nEncoding
           >> m_Para.bCompressLevel
           >> m_Para.nCompressLevel
           >> m_Para.bNoJpeg
           >> m_Para.nQualityLevel
           ;
    m_Para.nColorLevel = static_cast<CConnectTigerVnc::COLOR_LEVEL>(nColorLevel);
    return 0;
}
