#include "Connect.h"
#include <QApplication>
#include <QDebug>
#include <QtPlugin>

CConnect::CConnect(CFrmViewer *pView, QObject *parent)
    : QObject(parent), m_pView(nullptr)
{
    SetViewer(pView);
}

CConnect::~CConnect()
{
    qDebug() << "CConnect::~CConnect()";
}

int CConnect::SetViewer(CFrmViewer *pView)
{
    if(!pView) return -1;
    
    if(m_pView)
    {
        this->disconnect(m_pView);
    }
    
    m_pView = pView;
    bool check = connect(this, SIGNAL(sigConnected()),
                         m_pView, SLOT(slotConnect()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigDisconnected()),
                    m_pView, SLOT(slotDisconnect()));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigSetDesktopSize(int, int)),
                    m_pView, SLOT(slotSetDesktopSize(int, int)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigSetDesktopName(const QString&)),
                    m_pView, SLOT(slotSetName(const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigServerCutText(const QString&)),
                    m_pView, SLOT(slotServerCutText(const QString&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateRect(const QRect&, const QImage&)),
                    m_pView, SLOT(slotUpdateRect(const QRect&, const QImage&)));
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigUpdateCursor(const QRect&, const QImage&)),
                    m_pView, SLOT(slotUpdateCursor(const QRect&, const QImage&)));
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

QString CConnect::GetDescription()
{
    if(m_szServerName.isEmpty())
        return m_szHost + ":" + QString::number(m_nPort);
    return m_szServerName;
}

int CConnect::SetServerName(const QString &szServerName)
{
    m_szServerName = szServerName;
    m_pView->setWindowTitle(GetDescription());
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
