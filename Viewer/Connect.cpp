// Author: Kang Lin <kl222@126.com>

#include "FrmViewer.h"
#include "Connect.h"
#include <QApplication>
#include <QDebug>
#include <QtPlugin>
#include <QClipboard>
#include <QTimer>
#include "RabbitCommonLog.h"

int g_QtKeyboardModifiers = qRegisterMetaType<Qt::KeyboardModifiers>();
int g_QtMouseButtons = qRegisterMetaType<Qt::MouseButtons>();

CConnect::CConnect(CConnecter *pConnecter, QObject *parent)
    : QObject(parent), m_pView(nullptr)
{
    Q_ASSERT(pConnecter);
    bool check = connect(QApplication::clipboard(), SIGNAL(dataChanged()),
                         this, SLOT(slotClipBoardChange()), Qt::DirectConnection);
    Q_ASSERT(check);
    CFrmViewer* pView = qobject_cast<CFrmViewer*>(pConnecter->GetViewer());
    if(pView)
        SetViewer(pView);
    SetConnecter(pConnecter);
}

CConnect::~CConnect()
{
    qDebug() << "CConnect::~CConnect()";
}

int CConnect::Process()
{
    return 0;
}

void CConnect::slotTimeOut()
{
    try {
        // >= 0 : continue
        // <  0: error
        int nTime = Process();
        if(nTime >= 0)
        {
            QTimer::singleShot(nTime, this, SLOT(slotTimeOut()));
            return;
        }
    } catch(std::exception e) {
        LOG_MODEL_ERROR("CConnect", "process fail:%s", e.what());
        emit sigError(-1, e.what());
    }  catch (...) {
        LOG_MODEL_ERROR("CConnect", "process fail");
        emit sigError(-2, "");
    }
    emit sigDisconnected();
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
    check = connect(this, SIGNAL(sigUpdateCursorPosition(const QPoint&)),
                    m_pView, SLOT(slotUpdateCursorPosition(const QPoint&)));
    Q_ASSERT(check);
    
    check = connect(m_pView, SIGNAL(sigMousePressEvent(Qt::MouseButtons, QPoint)),
                    this, SLOT(slotMousePressEvent(Qt::MouseButtons, QPoint)));
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigMouseReleaseEvent(Qt::MouseButton, QPoint)),
                    this, SLOT(slotMouseReleaseEvent(Qt::MouseButton, QPoint)));
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigMouseMoveEvent(Qt::MouseButtons, QPoint)),
                    this, SLOT(slotMouseMoveEvent(Qt::MouseButtons, QPoint)));
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigWheelEvent(Qt::MouseButtons, QPoint, QPoint)),
                    this, SLOT(slotWheelEvent(Qt::MouseButtons, QPoint, QPoint)));
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigKeyPressEvent(int, Qt::KeyboardModifiers)),
                    this, SLOT(slotKeyPressEvent(int, Qt::KeyboardModifiers)));
    Q_ASSERT(check);
    check = connect(m_pView, SIGNAL(sigKeyReleaseEvent(int, Qt::KeyboardModifiers)),
                    this, SLOT(slotKeyReleaseEvent(int, Qt::KeyboardModifiers)));
    Q_ASSERT(check);
        
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

void CConnect::slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta)
{
    qDebug() << "CConnect::slotWheelEvent";
}

void CConnect::slotMouseMoveEvent(Qt::MouseButtons, QPoint)
{
    qDebug() << "CConnect::slotMouseMoveEvent";
}

void CConnect::slotMousePressEvent(Qt::MouseButtons, QPoint)
{
    qDebug() << "CConnect::slotMousePressEvent";
}

void CConnect::slotMouseReleaseEvent(Qt::MouseButton, QPoint)
{
    qDebug() << "CConnect::slotMouseReleaseEvent";
}

void CConnect::slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers)
{
    qDebug() << "CConnect::slotKeyPressEvent";
}

void CConnect::slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers)
{
    qDebug() << "CConnect::slotKeyReleaseEvent";
}
