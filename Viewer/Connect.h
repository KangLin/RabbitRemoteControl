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

/*!
 * \~chinese
 * \brief 具体连接接口。
 *     它默认启动一个定时器来开启一个非 Qt 事件循环（就是普通的循环处理）。
 *     详见： Connect() slotTimeOut() OnProcess() 。
 *     当然，它仍然支持 Qt 事件（QObject 的 信号 － 槽 机制）。
 * \note 这个接口仅由插件实现。 \n
 *     具体的插件需要实现下面接口：
 *     - OnInit()
 *     - OnClean()
 *     - OnProcess()
 *     - slotClipBoardChange()
 * 
 * \~english
 * \brief Connect interface.
 *      It starts a timer by default to start a non-Qt event loop (that is, normal loop processing) .
 *      See Connect(), slotTimeOut(), OnProcess() for details.
 *      Of course, it still supports Qt events (the signal-slot mechanism of QObject).
 * \note The interface only is implemented by plug-in. \n
 *      The plug-in needs to implement the following interface. 
 *     - OnInit()
 *     - OnClean()
 *     - OnProcess()
 *     - slotClipBoardChange()
 *
 * \~
 * \see CConnecterDesktop CConnecter CFrmViewer
 * \ingroup VIEWER_PLUGIN_API
 */
class VIEWER_EXPORT CConnect : public QObject
{
    Q_OBJECT

public:
    /*!
     * \~chinese
     * \param pConnecter
     * \param parent
     * \param bDirectConnection:
     *        \li true: 当可能会阻塞事件循环时
     *        \li false: 当一个非阻塞事件循环时
     * \~english
     * \param pConnecter
     * \param parent
     * \param bDirectConnection:
     *        \li true: when connect is non-Qt event and it can block event loop
     *        \li false: The connect is Qt event and it isn't block event loop
     */
    explicit CConnect(CConnecter* pConnecter,
                      QObject *parent = nullptr,
                      bool bDirectConnection = true);
    virtual ~CConnect() override;

public Q_SLOTS:   
    /*!
     * \~chinese 默认开始定时器
     * \~english Default start timer.
     * \~
     * \return 
     *     \li < 0 : error
     *     \li = 0 : emit sigConnected by caller
     *     \li = 1 : emit sigConnected in CConnect
     * \see OnProcess slotTimeOut
     */
    virtual int Connect();
    virtual int Disconnect();

    /*!
     * \~chinese 当剪切板发生改变时调用
     * \~english Be called whe the clip board change
     */
    virtual void slotClipBoardChange() = 0;
    
protected:
    virtual int SetConnecter(CConnecter* pConnecter);
    virtual int SetViewer(CFrmViewer* pView, bool bDirectConnection);
    virtual int SetParamter(void *pPara);

    /*!
     * \~chinese 具体的插件实现连接初始化
     * \~english Specific plug-in realizes connection initialization
     * \~
     * \return 
     *     \li < 0 : error
     *     \li = 0 : emit sigConnected by caller
     *     \li = 1 : emit sigConnected in CConnect
     * \see Connect()
     */
    virtual int OnInit() = 0;
    /*!
     * \~chinese 清理
     * \~english Clean
     * \~
     * \see Disconnect()
     */
    virtual int OnClean() = 0;
    /*!
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
    /*!
     * \~chinese 一个非 Qt 事件处理，它调用 OnProcess()，并根据其返回值开始新的定时器。
     *   如果 CConnect 没有一个非 Qt 事件循环（就是普通的循环处理），必须重载它。
     *   参见 CConnectTigerVnc::slotTimeOut()
     * \~english a non-Qt event loop (that is, normal loop processing)，
     *   It call OnProcess(), and start timer.
     *   If CConnect don not have a non-Qt event loop, must override it.
     *   see CConnectTigerVnc::slotTimeOut()
     */
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
