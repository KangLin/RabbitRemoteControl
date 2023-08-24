// Author: Kang Lin <kl222@126.com>
  
#ifndef CCONNECT_H
#define CCONNECT_H

#pragma once

#include <QPoint>
#include <QtPlugin>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include "Connecter.h"
#include <QMimeData>
#include "FrmViewer.h"
#include <QMessageBox>

/*!
 * \~chinese
 * \brief 具体连接接口。它由协议插件实现。
 *     它默认启动一个定时器来开启一个非 Qt 事件循环（就是普通的循环处理）。
 *     详见： Connect()、 slotTimeOut()、 OnProcess() 。
 *     当然，它仍然支持 Qt 事件（QObject 的 信号 － 槽 机制）。
 * \note
 *  - 这个接口仅由插件实现。
 *  - 它的实例在后台线程中。
 *  - 具体的插件需要实现下面接口：
 *     - OnInit()
 *     - OnClean()
 *     - OnProcess()
 *     - slotClipBoardChanged()
 * 
 * \~english
 * \brief Connect interface. It is implemented by the Protocol plugin.
 *      It starts a timer by default to start a non-Qt event loop (that is, normal loop processing) .
 *      See Connect(), slotTimeOut(), OnProcess() for details.
 *      Of course, it still supports Qt events (the signal-slot mechanism of QObject).
 * \note
 *  - The interface only is implemented by plug-in.
 *  - Its instance is in a background thread.
 *  - The plug-in needs to implement the following interface.
 *     - OnInit()
 *     - OnClean()
 *     - OnProcess()
 *     - slotClipBoardChanged()
 *
 * \~
 * \see CConnecterDesktopThread CFrmViewer
 * \ingroup CLIENT_PLUGIN_API
 */
class CLIENT_EXPORT CConnect : public QObject
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
     * \see OnProcess slotTimeOut
     */
    virtual int Connect();
    virtual int Disconnect();

    /*!
     * \~chinese 当剪切板发生改变时调用
     * \~english Be called when the clip board change
     */
    virtual void slotClipBoardChanged() = 0;

protected:
    virtual int SetConnecter(CConnecter* pConnecter);
    virtual int SetViewer(CFrmViewer* pView, bool bDirectConnection);

    /*!
     * \~chinese 具体的插件实现连接初始化
     * \return
     * \li < 0: 错误
     * \li = 0: 使用 OnProcess (非 Qt 事件循环)
     * \li > 0: 不便用 use OnProcess (qt 事件循环)
     *
     * \~english Specific plug-in realizes connection initialization
     * \return
     * \li < 0: error
     * \li = 0: Use OnProcess (non-Qt event loop)
     * \li > 0: Don't use OnProcess (qt event loop)
     *
     * \~
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
     * \return 
     *       \li >= 0: 继续。调用间隔时间，单位毫秒
     *       \li <  0: 错误或停止
     *     
     * \~english Specific operation processing of plug-in connection
     * \return 
     *       \li >= 0: continue, Interval call time (msec)
     *       \li <  0: error or stop
     * \~
     * \see slotTimeOut()
     */
    virtual int OnProcess();

protected Q_SLOTS:
    /*!
     * \~chinese 一个非 Qt 事件处理，它调用 OnProcess()，并根据其返回值开始新的定时器。
     *   如果 CConnect 没有一个非 Qt 事件循环（就是普通的循环处理），必须重载它。
     *   参见 CConnectTigerVnc::slotTimeOut()
     *
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

    /*!
     * \brief When a background thread blocks the display window
     * \param className: show windows class name
     *        The class must have follower public functions:
     *            Q_INVOKABLE void SetContext(void* pContext);
     *            Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);
     * \param nRet: If className is QDialog derived class, QDialog::exec() return value.
     *              Otherwise, ignore
     * \param pContext: pass context to CConnecter::slotBlockShowWidget()
     * \see CConnecter::slotBlockShowWidget() SetConnecter
     */
    void sigBlockShowWidget(const QString& className, int &nRet, void* pContext);
    /*!
     * \brief The background thread uses QMessageBox to block the display window
     * \param title
     * \param message
     * \param buttons
     * \param nRet
     * \param checkBox
     * \param checkBoxContext
     * \see CConnecter::slotBlockShowMessage() SetConnecter
     */
    void sigBlockShowMessage(QString title, QString message,
                             QMessageBox::StandardButtons buttons,
                             QMessageBox::StandardButton& nRet,
                             bool &checkBox,
                             QString checkBoxContext = QString());

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
