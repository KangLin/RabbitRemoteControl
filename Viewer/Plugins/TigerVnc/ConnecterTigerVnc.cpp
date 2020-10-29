#include "ConnecterTigerVnc.h"
#include "rfb/encodings.h"
#include <QDebug>

CConnecterTigerVnc::CConnecterTigerVnc(QObject *parent)
    : CConnecter(parent),
      m_pThread(nullptr)
{
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
        bool check = connect(this, SIGNAL(destroyed()),
                           m_pThread, SLOT(deleteLater()));
        Q_ASSERT(check);
    }
    m_pThread->start();
    return 0;
}

int CConnecterTigerVnc::DisConnect()
{
    m_pThread->m_bExit = true;
    m_pThread->quit();
    return 0;
}
