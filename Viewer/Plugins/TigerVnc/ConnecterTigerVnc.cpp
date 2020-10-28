#include "ConnecterTigerVnc.h"
#include "rfb/encodings.h"

CConnecterTigerVnc::CConnecterTigerVnc(QObject *parent)
    : CConnecter(parent),
      m_pThread(nullptr)
{
    m_Para.nEncoding = rfb::encodingTight;
    m_Para.nCompressLevel = 2;
    m_Para.nQualityLevel = -1;

    m_pView = new CFrmViewer();
}

CConnecterTigerVnc::~CConnecterTigerVnc()
{}

CFrmViewer* CConnecterTigerVnc::GetViewer()
{
    return m_pView;
}

QDialog *CConnecterTigerVnc::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettings(&m_Para);
}

int CConnecterTigerVnc::Connect()
{
    if(nullptr == m_pThread)
    {
        m_pThread = new CConnectThread(m_pView, this);
        bool check = connect(m_pThread, SIGNAL(destroyed()),
                           m_pThread, SLOT(deleteLater()));
        Q_ASSERT(check);
    }
    m_pThread->start();
    return 0;
}

int CConnecterTigerVnc::DisConnect()
{
    m_pThread->quit();
    return 0;
}
