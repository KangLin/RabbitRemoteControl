// Author: Kang Lin <kl222@126.com>
  
#ifndef CCONNECT_H
#define CCONNECT_H

#pragma once

#include <QObject>
#include <QPoint>
#include <QtPlugin>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include "Connecter.h"
#include <QMimeData>
#include "FrmViewer.h"

/**
 * \~chinese
 * \brief 具体连接接口
 * 它默认启动一个定时器来模拟事件循环。详见 Connect()
 * \note 这个接口仅由插件实现
 * 
 * \~english
 * \brief Connect interface.
 *  It starts a timer by default to simulate the event loop.
 *  See Connect() for details 
 * \note The interface only is implemented by plugin
 * 
 * \~
 * \see  CConnecter CFrmViewer
 * \ingroup VIEWER_PLUGIN_API
 */
class VIEWER_EXPORT CConnect : public QObject
{
    Q_OBJECT

public:
    explicit CConnect(CConnecter* pConnecter, QObject *parent = nullptr);
    virtual ~CConnect() override;

public Q_SLOTS:   
    /**
     * \~chinese 默认开始定时器
     * \~english Default start timer.
     * \~
     * \return 
     *     \li < 0 : error
     *     \li = 0 : emit sigConnected
     *     \li = 1 : emit sigConnected in CConnect
     * \see OnProcess slotTimeOut
     */
    virtual int Connect();
    virtual int Disconnect();

    virtual void slotClipBoardChange() = 0;
    
protected:
    virtual int SetConnecter(CConnecter* pConnecter);
    virtual int SetViewer(CFrmViewer* pView);
    virtual int SetParamter(void *pPara);

    /**
     * \~chinese 具体的插件实现连接初始化
     * \~english Specific plug-in realizes connection initialization
     * \~
     * \return 
     *     \li < 0 : error
     *     \li = 0 : emit sigConnected
     *     \li = 1 : emit sigConnected in CConnect
     * \see Connect()
     */
    virtual int OnInit() = 0;
    virtual int OnClean() = 0;
    /**
     * \~chinese 插件连接的具体操作处理
     * \~english Specific operation processing of plug-in connection
     * \~
     * \return 
     *       \li >= 0: continue, Interval call time (msec)
     *       \li <  0: error or stop
     * \see slotTimeOut()
     */
    virtual int OnProcess() = 0;

protected Q_SLOTS:
    ///
    /// \brief Timing start OnProcess(), it is main process in thread.
    /// If the CConnect is not main process, must override it.
    ///
    virtual void slotTimeOut();
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();

    void sigSetDesktopSize(int width, int height);
    void sigServerName(const QString& szName);

    void sigUpdateRect(const QRect& r, const QImage& image);
    void sigUpdateCursor(const QCursor& cursor);
    void sigUpdateCursorPosition(const QPoint& pos);
    void sigSetClipboard(QMimeData* data);

    void sigError(const int nError, const QString &szError = QString());
    void sigInformation(const QString& szInfo);

public Q_SLOTS:
    virtual void slotMousePressEvent(Qt::MouseButtons, QPoint);
    virtual void slotMouseReleaseEvent(Qt::MouseButton, QPoint);
    virtual void slotMouseMoveEvent(Qt::MouseButtons, QPoint);
    virtual void slotWheelEvent(Qt::MouseButtons buttons, QPoint pos, QPoint angleDelta);
    virtual void slotKeyPressEvent(int key, Qt::KeyboardModifiers modifiers);
    virtual void slotKeyReleaseEvent(int key, Qt::KeyboardModifiers modifiers);

private:
    CFrmViewer* m_pView;
};

#endif // CCONNECT_H
