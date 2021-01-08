//! @author: Kang Lin(kl222@126.com)

#include "Connect.h"
#include <QApplication>
#include <QDebug>
#include <QtPlugin>
#include <QClipboard>

CConnect::CConnect(CConnecter *pConnecter, QObject *parent)
    : QObject(parent), m_nPort(0), m_pView(nullptr)
{
    Q_ASSERT(pConnecter);
    bool check = connect(QApplication::clipboard(), SIGNAL(dataChanged()),
                         this, SLOT(slotClipBoardChange()), Qt::DirectConnection);
    Q_ASSERT(check);
    
    SetViewer(pConnecter->GetViewer());
    SetConnecter(pConnecter);
}

CConnect::~CConnect()
{
    qDebug() << "CConnect::~CConnect()";
}

int CConnect::SetConnecter(CConnecter* pConnecter)
{
    Q_ASSERT(pConnecter);
    if(!pConnecter) return -1;
    
    bool check = connect(this, SIGNAL(sigConnected()),
                         pConnecter, SIGNAL(sigConnected()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnected()),
                    pConnecter, SIGNAL(sigDisconnected()));
    Q_ASSUME(check);
    //NOTO: Prevent recursion when writing plugins
    check = connect(pConnecter, SIGNAL(sigDisconnected()),
            this, SLOT(Disconnect()), Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigServerName(const QString&)),
                    pConnecter, SLOT(slotSetServerName(const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigError(const int, const QString&)),
                    pConnecter, SIGNAL(sigError(const int, const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigInformation(const QString&)),
                    pConnecter, SIGNAL(sigInformation(const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigSetClipboard(QMimeData*)),
                    pConnecter, SLOT(slotSetClipboard(QMimeData*)));
    Q_ASSERT(check);
    return 0;
}

int CConnect::SetViewer(CFrmViewer *pView)
{
    Q_ASSERT(pView);
    if(!pView) return -1;
    
    if(m_pView)
    {
        this->disconnect(m_pView);
    }
    
    m_pView = pView;
    bool check = connect(this, SIGNAL(sigSetDesktopSize(int, int)),
                    m_pView, SLOT(slotSetDesktopSize(int, int)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigServerName(const QString&)),
                    m_pView, SLOT(slotSetName(const QString&)));
    Q_ASSERT(check);

    check = connect(this, SIGNAL(sigUpdateRect(const QRect&, const QImage&)),
                    m_pView, SLOT(slotUpdateRect(const QRect&, const QImage&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateCursor(const QCursor&)),
                    m_pView, SLOT(slotUpdateCursor(const QCursor&)));
    Q_ASSERT(check);
    
    check = connect(m_pView, SIGNAL(sigMousePressEvent(QMouseEvent*)),
                    this, SLOT(slotMousePressEvent(QMouseEvent*)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigMouseReleaseEvent(QMouseEvent*)),
                    this, SLOT(slotMouseReleaseEvent(QMouseEvent*)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigMouseMoveEvent(QMouseEvent*)),
                    this, SLOT(slotMouseMoveEvent(QMouseEvent*)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigWheelEvent(QWheelEvent*)),
                    this, SLOT(slotWheelEvent(QWheelEvent*)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigKeyPressEvent(QKeyEvent*)),
                    this, SLOT(slotKeyPressEvent(QKeyEvent*)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigKeyReleaseEvent(QKeyEvent*)),
                    this, SLOT(slotKeyReleaseEvent(QKeyEvent*)),
                    Qt::DirectConnection);
    Q_ASSERT(check);
        
    return 0;
}

int CConnect::SetServerName(const QString &szServerName)
{
    m_szServerName = szServerName;
    //TODO: add ipv6
    int nPos = szServerName.indexOf(":");
    if(-1 == nPos)
        m_szHost = szServerName;
    else {
        m_szHost = szServerName.left(nPos);
        m_nPort = szServerName.right(szServerName.length() - nPos - 1).toInt();
    }

    emit sigServerName(m_szServerName);
    return 0;
}

int CConnect::SetServer(const QString &szHost, const int nPort)
{
    m_szHost = szHost;
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

int CConnect::Initialize()
{
    return 0;
}

int CConnect::Clean()
{
    return 0;
}

void CConnect::slotWheelEvent(QWheelEvent*)
{
    qDebug() << "CConnect::slotWheelEvent";
}

void CConnect::slotMouseMoveEvent(QMouseEvent*)
{
    qDebug() << "CConnect::slotMouseMoveEvent";
}

void CConnect::slotMousePressEvent(QMouseEvent*)
{
    qDebug() << "CConnect::slotMousePressEvent";
}

void CConnect::slotMouseReleaseEvent(QMouseEvent*)
{
    qDebug() << "CConnect::slotMouseReleaseEvent";
}

void CConnect::slotKeyPressEvent(QKeyEvent*)
{
    qDebug() << "CConnect::slotKeyPressEvent";
}

void CConnect::slotKeyReleaseEvent(QKeyEvent*)
{
    qDebug() << "CConnect::slotKeyReleaseEvent";
}
