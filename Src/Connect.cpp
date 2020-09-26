#include "Connect.h"

CConnect::CConnect(CFrmViewer *pView, QObject *parent) : QObject(parent)
{
    m_pView = pView;
    m_bExit = false;
    m_bShared = true;
    m_bReDesktopSize = true;
    m_bUseLocalCursor = false;
    
    if(!pView) return;
    
    pView->SetConnect(this);
    bool check = connect(this, SIGNAL(sigConnected()),
                         pView, SLOT(slotConnect()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnect()),
                    pView, SLOT(slotDisconnect()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigSetDesktopSize(int, int)),
                    pView, SLOT(slotSetDesktopSize(int, int)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigServerCutText(const QString&)),
                    pView, SLOT(slotServerCutText(const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(slotUpdateRect(const QRect&, const QImage&)),
                    pView, SLOT(slotUpdateRect(const QRect&, const QImage&)));
    Q_ASSERT(check);
}

CConnect::~CConnect()
{}

int CConnect::SetServer(const QString &szIp, const int nPort)
{
    m_szIp = szIp;
    m_nPort = nPort;    
    return 0;
}

int CConnect::SetUser(const QString &szUser, const QString &szPassword)
{
    m_szUser = szUser;
    m_szPassword = szPassword;
    return 0;
}

int CConnect::SetParamter(void *pPara)
{
    Q_UNUSED(pPara)
    return 0;
}

int CConnect::SetShared(bool shared)
{
    m_bShared = shared;
    return 0;
}

bool CConnect::GetShared()
{
    return m_bShared;
}

int CConnect::SetReDesktopSize(bool re)
{
    m_bReDesktopSize = re;
    return 0;
}

bool CConnect::GetReDesktopSize()
{
    return m_bReDesktopSize;
}

int CConnect::SetUseLocalCursor(bool u)
{
    m_bUseLocalCursor = u;
    return 0;
}

bool CConnect::GetUserLocalCursor()
{
    return m_bUseLocalCursor;
}

int CConnect::Initialize()
{
    return 0;
}

int CConnect::Connect()
{
    return 0;
}

int CConnect::Disconnect()
{
    m_bExit = true;
    return 0;
}

int CConnect::Exec()
{
    return 0;
}
