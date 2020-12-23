#include "ConnecterLibVnc.h"
#include "DlgSettingsLibVnc.h"
#include <QDebug>

CConnecterLibVnc::CConnecterLibVnc(QObject *parent) : CConnecter(parent),
    m_pThread(nullptr)
{
}

CConnecterLibVnc::~CConnecterLibVnc()
{
    qDebug() << "CConnecterLibVnc::~CConnecterLibVnc()";
    if(m_pThread)
    {
        m_pThread->wait();
        delete m_pThread;
    }
}

QString CConnecterLibVnc::ServerName()
{
    //TODO: add server name
    return QString();
}

QString CConnecterLibVnc::Name()
{
    return "LibVnc";
}

QString CConnecterLibVnc::Description()
{
    return Protol() + ":";
}

QString CConnecterLibVnc::Protol()
{
    return "RFB";
}

qint16 CConnecterLibVnc::Version()
{
    return 0;
}

QDialog *CConnecterLibVnc::GetDialogSettings(QWidget *parent)
{
    return new CDlgSettingsLibVnc(this, parent);
}

int CConnecterLibVnc::Load(QDataStream &d)
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterLibVnc::Save(QDataStream &d)
{
    int nRet = 0;
    
    return nRet;
}

int CConnecterLibVnc::Connect()
{
    int nRet = 0;
    if(nullptr == m_pThread)
    {
        m_pThread = new CConnectThreadLibVnc(GetViewer(), this);
    }
    m_pThread->start();
    return nRet;
}

int CConnecterLibVnc::DisConnect()
{
    if(!m_pThread) return -1;
    
    m_pThread->m_bExit = true;
    // Actively disconnect, without waiting for the thread to exit and then disconnect
    emit sigDisconnected();
    return 0;
}

void CConnecterLibVnc::slotSetClipboard(QMimeData *data)
{
}
