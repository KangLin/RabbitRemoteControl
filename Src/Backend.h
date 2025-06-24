// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include "Operate.h"

/*!
 * \~chinese
 * \brief 后端接口。它由协议插件实现。
 *     它默认启动一个定时器来开启一个非 Qt 事件循环（就是普通的循环处理）。
 *     详见： Start()、 slotTimeOut()、 OnProcess() 。
 *     当然，它仍然支持 Qt 事件（QObject 的 信号 － 槽 机制）。
 * \note
 *  - 这个接口仅由插件实现。
 *  - 如果是异步的（有后台线程），它的实例在后台线程中。
 *  - 具体的插件需要实现下面接口：
 *     - OnInit()
 *     - OnClean()
 *     - OnProcess()
 * 
 * \~english
 * \brief Backend interface. It is implemented by the Protocol plugin.
 *      It starts a timer by default to start a non-Qt event loop
 *      (that is, normal loop processing) .
 *      See Start(), slotTimeOut(), OnProcess() for details.
 *      Of course, it still supports Qt events (the signal-slot mechanism of QObject).
 * \note
 *  - The interface only is implemented by plug-in.
 *  - If it's asynchronous (with a background thread),
 *    its instance is in a background thread.
 *  - The plug-in needs to implement the following interface.
 *     - OnInit()
 *     - OnClean()
 *     - OnProcess()
 *
 * \~
 * \see CBackendThread::run() CFrmViewer
 * \ingroup PLUGIN_API
 */
class PLUGIN_EXPORT CBackend : public QObject
{
    Q_OBJECT
public:
    explicit CBackend(COperate *pOperate = nullptr);
    virtual ~CBackend();

public Q_SLOTS:
    /*!
     * \~chinese 开始。根据　OnInit()　返回值来决定是否开始定时器来支持非 qt 事件
     * \note
     * - 如果是异步的，开始成功后触发 sigRunning() 。
     * - 如果是同步的，则在此函数成功调用后，触发 sigRunning() 。
     *
     * \~english Start. Based on the OnInit() return value,
     *           decide whether to start the timer to support no-qt event
     * \note
     * - If it is asynchronous,
     *   sigRunning() is triggered after the start is success.
     * - If it is synchronous,
     *   sigRunning() is triggered after this function is successfully called.
     *
     * \~
     * \return
     *   - = 0: success
     *   - < 0: error
     * \see sigRunning()
     * \see OnInit() OnProcess() slotTimeOut()
     */
    virtual int Start();
    /*!
     * \~chinese 停止
     * \~english Stop
     * \~
     * \return 0: success; other error
     * \see sigFinished()
     */
    virtual int Stop();
    
protected:
    enum class OnInitReturnValue {
        Fail = -1,
        Success = 0,
        UseOnProcess = Success,
        NotUseOnProcess = 1,
    };
    /*!
     * \~chinese 初始化
     * \return
     * \li < 0: 错误
     * \li = 0: 使用 OnProcess() (非 Qt 事件循环)
     * \li > 0: 不使用 OnProcess() (qt 事件循环)
     *
     * \~english Initialization
     * \return
     * \li < 0: error
     * \li = 0: Use OnProcess() (non-Qt event loop)
     * \li > 0: Don't use OnProcess() (qt event loop)
     *
     * \~
     * \see Start()
     */
    [[nodiscard]] virtual OnInitReturnValue OnInit() = 0;
    /*!
     * \~chinese 清理
     * \~english Clean
     * \~
     * \see Stop()
     */
    virtual int OnClean() = 0;
    /*!
     * \~chinese 具体操作处理
     * \return 
     *       \li >= 0: 继续。再次调用间隔时间，单位毫秒
     *       \li = -1: 停止
     *       \li < -1: 错误
     *     
     * \~english Specific operation processing of plug-in
     * \return 
     *       \li >= 0: continue, Interval call time (msec)
     *       \li = -1: stop
     *       \li < -1: error
     * \~
     * \see Start() slotTimeOut()
     */
    [[nodiscard]] virtual int OnProcess();
    
protected Q_SLOTS:
    /*!
     * \~chinese 一个非 Qt 事件处理，它调用 OnProcess()，并根据其返回值开始新的定时器。
     *   如果是不是一个非 Qt 事件循环（就是普通的循环处理），
     *   可以重载它，或者 OnInit() 返回值大于 0
     *
     * \~english a non-Qt event loop (that is, normal loop processing)，
     *   It call OnProcess(), and start timer.
     *   If it is not have a non-Qt event loop,
     *   can override it, or OnInit() return >0
     *
     * \~
     * \see Start() OnProcess()
     */
    virtual void slotTimeOut();
    
Q_SIGNALS:
    /*!
     * \~chinese 当插件开始成功后触发。仅由插件触发
     * \~english Emitted when the plugin is successfully started.
     *           Emitted only by plugins
     * \see Start()
     */
    void sigRunning();
    /*!
     * \~chinese 需要通知用户停止时触发。仅由插件触发。
     *    当从插件中需要停止时触发。例如：对端断开连接、重置连接或者连接出错。
     *
     * \~english Notify the user to stop. Emitted only by plugins
     *    Emitted when you need to stop from the plug-in.
     *    For example, the peer disconnect or reset the connection
     *    or the connection is error
     */
    void sigStop();
    /*!
     * \~chinese 停止成功信号。仅由插件触发
     * \~english Successful stopped signal. Triggered only by plugins
     * \~
     * \see Stop
     */
    void sigFinished();
    /*!
     * \~chinese 当有错误产生时触发
     * \~english Triggered when an error is generated
     */
    void sigError(const int nError, const QString &szError = QString());
    /*!
     * \~chinese 从后台线程中触发在主线程中显示信息，不阻塞后台线程
     * \note 它与 sigShowMessageBox 的区别是 sigShowMessageBox 用对话框显示
     *
     * \~english Triggering from a background thread displays information
     *           in the main thread without blocking the background thread
     * \note It differs from sigShowMessageBox in that sigShowMessageBox
     *       is displayed in a dialog box
     *
     * \~
     * \see sigShowMessageBox SetConnect COperate::sigInformation()
     */
    void sigInformation(const QString& szInfo);
    /*!
     * \~chinese
     * \brief 从后台线程中触发在主线程中显示消息对话框(QMessageBox)，不阻塞后台线程
     * \note  它与 sigInformation 区别是，sigInformation 不用对话框显示
     *
     * \~english
     * \brief Trigger the display of a message dialog (QMessageBox)
     *        in the main thread from a background thread
     *        without blocking the background thread
     * \note It differs from sigInformation in that sigInformation
     *       is not displayed in a dialog box
     *
     * \~
     * \see sigInformation SetConnect COperate::sigShowMessageBox()
     */
    void sigShowMessageBox(const QString& szTitle, const QString& szMessage,
                           const QMessageBox::Icon& icon = QMessageBox::Information);

    /*!
     * \~chinese
     * 阻塞后台线程，并在前台线程中显示消息对话框(QMessageBox)
     *
     * \~english
     * \brief Block background threads and display message dialogs
     *        in foreground threads (QMessageBox)
     * \param title
     * \param message
     * \param buttons
     * \param nRet
     * \param checkBox
     * \param checkBoxContext
     * 
     * \~
     * \see COperate::slotBlockShowMessageBox() SetConnect
     */
    void sigBlockShowMessageBox(const QString& szTitle,
                                const QString& szMessage,
                                QMessageBox::StandardButtons buttons,
                                QMessageBox::StandardButton& nRet,
                                bool &checkBox,
                                QString checkBoxContext = QString());
    /*!
     * \~chinese 阻塞后台线程，并在前台线程中显示输入对话框 (QInputDialog)
     *
     * \~english
     * \brief Block background threads and display input dialogs
     *        in foreground threads (QInputDialog)
     * \~
     * \see COperate::slotBlockInputDialog() SetConnect
     */
    void sigBlockInputDialog(const QString& szTitle,
                             const QString& szLable,
                             const QString& szMessage,
                             QString& szText
                             );
    /*!
     * \~chinese
     * 阻塞后台线程，并在前台线程中显示窗口。
     *
     * \~english
     * \brief Blocks the background thread and displays the window
     *        in the foreground thread.
     * \param className: show windows class name
     *        The class must have follower public functions:
     *            Q_INVOKABLE void SetContext(void* pContext);
     * \param nRet: If className is QDialog derived class,
     *              QDialog::exec() return value. Otherwise, ignore
     * \param pContext: pass context to COperate::slotBlockShowWidget()
     *
     * \~
     * \see COperate::slotBlockShowWidget() SetConnect
     */
    void sigBlockShowWidget(const QString& className, int &nRet, void* pContext);

private:
    int SetConnect(COperate* pOperate);
};
