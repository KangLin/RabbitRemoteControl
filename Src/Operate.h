// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QMessageBox>
#include <QObject>
#include <QMenu>
#include <QClipboard>
#include "ParameterPlugin.h"
#include "Stats.h"

class CPlugin;

/*!
 * \~chinese
 * \brief 操作接口。
 * \note
 *   - 此接口仅由是用户使用。它由插件实现。
 *   - 它的实例在主线程（界面线程）中。
 * \details 
 * 类图： \image html docs/Image/PluginAPI.svg
 * 序列图： \image html docs/Image/SequenceDiagram.svg
 *  已经提供以下类型的基本实现：\n
 *  - 远程桌面类： \ref COperateDesktop \n
 *  - 工具类： \n
 *    - 非阻塞模型（在主线程）： COperateWakeOnLan \n
 *  - 控制台类： \ref COperateTerminal \n
 *  - 文件传输： \ref COperateFileTransfer \n
 *  - 服务类： \ref COperateService \n
 *
 * \~english
 * \brief Operate interface
 * \note
 *   - This interface is only used by users. It is implemented by plugins.
 *   - Its instance is in the main thread.
 * \details
 * Class diagram: \image html docs/Image/PluginAPI.svg
 * Sequen diagram: \image html docs/Image/SequenceDiagram.svg
 * Basic implementations of the following types have been provided: \n
 *   - Remote desktop type: \n
 *   - Tool type: \n
 *     - Is a non-blocking model(in main thread): \ref COperateWakeOnLan \n
 *   - Termianal type: \ref CConnecterTerminal
 *   - File transfer: \ref COperateFileTransfer \n
 *
 * \~
 * \see CPlugin
 * \ingroup APP_API PLUGIN_API
 */
class PLUGIN_EXPORT COperate : public QObject
{
    Q_OBJECT

public:
    explicit COperate(CPlugin *plugin);
    virtual ~COperate();
    
    //!@{
    //! \~chinese \name 属性
    //! \~english \name Property

    //! Identity
    [[nodiscard]] virtual const QString Id();
    //! Name
    [[nodiscard]] virtual const QString Name();
    //! Description
    [[nodiscard]] virtual const QString Description();
    //! Protocol
    [[nodiscard]] virtual const QString Protocol() const;
    //! Version
    [[nodiscard]] virtual const qint16 Version() const = 0;
    //! Icon
    [[nodiscard]] virtual const QIcon Icon() const;
    //! Get type name
    [[nodiscard]] virtual const QString GetTypeName() const;
    //!@}

    /*!
     * \~chinese
     * \brief 得到显示视图
     * \return QWidget*: 视图指针。它的所有者是本类或其派生类的实例
     * \note 如果自己实现视图，则需要在开始时禁用视图，在　sigRunning　后允许视图。
     *
     * \~english
     * \brief Get Viewer
     * \return QWidget*: the ownership is a instance of this class or its derivative class
     * \note If you implement the view yourself, you need to disable the view
     *       at the beginning and allow the view after sigRun.
     *
     * \~
     * \see sigRunning CFrmViewer::CFrmViewer
     */
    [[nodiscard]] virtual QWidget* GetViewer() = 0;
    /*!
     * \~chinese
     * \brief 打开设置对话框
     * \param parent
     * \return DialogCode
     *   \li QDialog::Accepted: 接收
     *   \li QDialog::Rejected: 拒绝
     *   \li -1: 错误
     *
     * \~english
     * \brief Open settings dialog
     * \param parent
     * \return DialogCode
     *   \li QDialog::Accepted
     *   \li QDialog::Rejected
     *   \li -1: error
     */
    [[nodiscard]] virtual int OpenDialogSettings(QWidget* parent = nullptr);
    /*!
     * \brief Get menu
     */
    [[nodiscard]] virtual QMenu* GetMenu(QWidget* parent = nullptr);

    [[nodiscard]] virtual CStats* GetStats();

    enum class SecurityLevel {
        No,                       // No the function
        Secure,                   // Both authentication and channels are secure. Green
        NonSecureAuthentication,  // Non-secure authentication over a secure channel. Blue
        SecureChannel,            // Channel is secure. Yellow
        Normal = SecureChannel,
        SecureAuthentication,     // There is security verification, not a secure channel. Orange
        Risky                     // Red
    };
    [[nodiscard]] virtual SecurityLevel GetSecurityLevel();
    [[nodiscard]] virtual QString GetSecurityLevelString();
    [[nodiscard]] virtual QColor GetSecurityLevelColor();
    [[nodiscard]] static QString GetSecurityLevelString(SecurityLevel level);
    [[nodiscard]] static QColor GetSecurityLevelColor(SecurityLevel level);

public:
    /**
     * \~chinese
     * \brief 开始
     * \note 仅由用户调用，插件不能直接调用此函数。
     *       插件开始成功后，触发信号 sigRunning()
     *       
     * \~english
     * \brief Start
     * \note Only call by user, The plugin don't call it. 
     *       When plugin is started, it emit sigRunning()
     *       
     * \~
     * \see sigRunning()
     */
    virtual int Start() = 0;
    /*!
     * \~chinese
     * \brief 关闭
     * \note 仅由用户调用，插件不能直接调用此函数。
     *       插件停止成功后，触发信号 sigFinished()。调用者收到信号后，删除对象
     *       
     * \~english
     * \brief Stop
     * \note Only call by user, The plugin don't call it.
     *       When plugin is stopped, it emit sigFinished().
     *       The caller receive the signal, then delete the object.
     * 
     * \~
     * \see sigFinished()
     */
    virtual int Stop() = 0;

Q_SIGNALS:
    /*!
     * \~chinese 开始成功信号。仅由插件触发。
     *  - 应用程序需要在此时设置视图属性 Enabled 为 true 。允许接收键盘和鼠标事件。
     *  - 如果插件自己实现视图（非　CFrmView），设置视图属性 Enabled 为 true 。允许接收键盘和鼠标事件。
     *
     * \~english Start success signal. Triggered only by plugins.
     *  - The application needs to set the view property Enabled to true at this point.
     *   enable accept keyboard and mouse event
     *  - If the plugin implements its own view (non-CFrmView),
     *   set the view property Enabled to true. Allows receiving keyboard and mouse events.
     *
     * \~
     * \see MainWindow::slotRunning() GetViewer() CFrmViewer::CFrmViewer
     * \snippet App/Client/mainwindow.cpp MainWindow slotConnected
     */
    void sigRunning();
    /*!
     * \~chinese 通知用户停止。仅由插件触发。
     *    当从插件中需要停止时触发。例如：对端断开连接、重置连接或者连接出错。
     *    当应用接收到此信号后，调用 Stop() 关闭连接。
     * \~english Notify the user to stop. Triggered only by plugins
     *    Emit when you need to stop from the plug-in.
     *    For example, the peer disconnect or reset the connection
     *    or the connection is error.
     *    When the applicatioin receive the signal,
     *    call Stop() to close connect
     */
    void sigStop();
    /*!
     * \~chinese 断开连接成功信号。仅由插件触发
     * \~english Successful disconnection signal. Triggered only by plugins
     * \~
     * \see Disconnect()
     */
    void sigFinished();

    /*!
     * \~chinese 视图获得焦点
     * \~english The view is focus
     * \param pView
     */
    void sigViewerFocusIn(QWidget* pView);
    /*!
     * \~chinese \note 名称更新。此信号仅由本类触发
     * \~english \note The name is changed. This signal is only triggered by this class
     */
    void sigUpdateName(const QString& szName);
    /*!
     * \~chinese
     * \brief 更新参数，通知应用程序保存或显示参数
     * \note 插件不要直接使用它，请用 CParameter::sigChanged 触发此信号
     *
     * \~english
     * \brief Update parameters, notify application to save or show parameters.
     * \note The plugin don't use it directly.
     *       Please use CParameter::sigChanged to emit this signal.
     * \~
     * \see COperateDesktop::SetParameter
     */
    void sigUpdateParameters(COperate* pOperate);
    /*!
     * \~chinese 当有错误产生时触发。
     * \~english Triggered when an error is generated
     */
    void sigError(const int nError, const QString &szError);
    /*!
     * \~chinese
     * \note 它与 sigShowMessageBox 的区别是 sigShowMessageBox 用对话框显示
     * \brief 中主窗口中显示信息
     *
     * \~english
     * \note It differs from sigShowMessageBox in that sigShowMessageBox is displayed in a dialog box
     * \brief Show information in main windows 
     * \param szInfo
     * \~
     * \see sigShowMessageBox MainWindow::slotInformation()
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
     * \note It differs from sigInformation in that sigInformation is not displayed in a dialog box
     *
     * \~
     * \see sigInformation Connect::SetConnecter MainWindow::slotShowMessageBox
     */
    void sigShowMessageBox(const QString& title, const QString& message,
                           const QMessageBox::Icon& icon = QMessageBox::Information);

protected:
    /*!
     * \brief Initial parameters and resource
     * \~
     * \see CManager::CreateOperate
     */
    Q_INVOKABLE virtual int Initial();
    /*!
     * \brief Clean parameters and resource
     * \see CManager::DeleteOperate
     */
    Q_INVOKABLE virtual int Clean();
    /*!
     * \~chinese 应用插件全局参数
     * \note 如果不需要插件的全局参数，请在派生类中重载它，并忽略。
     *
     * \~english Apply the global parameters of the plug-in
     * \note If you don't need the global parameters of the plugin,
     *       override it in the derived class and ignore.
     * \see CManager::CreateOperate CParameterPlugin
     */
    Q_INVOKABLE virtual int SetGlobalParameters(CParameterPlugin* pPara) = 0;

protected:
    /*!
     * \brief Get plugin
     * \see CManager::DeleteOperate 
     */
    [[nodiscard]] Q_INVOKABLE CPlugin* GetPlugin() const;

    static QObject* createObject(const QString &className, QObject* parent = NULL);

private:
    /*!
     * \~chinese
     * \brief 得到设置对话框
     * \param parent: 返回窗口的父窗口
     * \return QDialog*: 对话框必须在堆内存中分配，它的所有者是调用者。
     *
     * \~english
     * \brief Open settgins dialog
     * \param parent: the parent windows of the dialog of return
     * \return QDialog*: then QDialog must be allocated in heap memory, the ownership is caller.
     *
     * \~
     * \see OpenDialogSettings
     */
    [[nodiscard]] virtual QDialog* OnOpenDialogSettings(QWidget* parent = nullptr) = 0;
    
    //!@{
    //! \~chinese \name 加载和保存参数
    //! \~english \name Load and save settings
public:
    /* \~chinese 得到配置文件名
     * \~english Get settings file name
     * \see CManager::SaveOperate
     */
    [[nodiscard]] virtual QString GetSettingsFile();
    /* \~chinese 设置配置文件名
     * \~english Set settings file name
     */
    virtual int SetSettingsFile(const QString &szFile);
private:
    QString m_szSettings;
protected:
    /*!
     * \~chinese \brief 加载参数
     * \~english \brief Load parameters
     */
    virtual int Load(QSettings &set);
    /*!
     * \~chinese 保存参数
     * \~english Save parameters
     */
    virtual int Save(QSettings &set);
    /*!
     * \~chinese \brief 从文件中加载参数
     * \~english \brief Load parameters from file
     * \~
     * \see CManger::LoadOperate
     */
    Q_INVOKABLE virtual int Load(QString szFile = QString());
    /*!
     * \~chinese \brief 保存参数到文件中
     * \param szFile: 文件名。
     * \~english Save parameters to file
     * \param szFile: File name.
     * \~
     * \see CManger::SaveOperate
     */
    Q_INVOKABLE virtual int Save(QString szFile = QString());
    //!@}

private Q_SLOTS:
    void slotUpdateName();

    /*!
     * \~chinese
     * 阻塞后台线程，并在前台线程中显示窗口。
     *
     * \~english
     * \brief Blocks the background thread and displays the window in the foreground thread.
     * \param className: show windows class name
     *        The class must have follower public functions:
     *            Q_INVOKABLE void SetContext(void* pContext);
     * \param nRet: If className is QDialog derived class, QDialog::exec() return value.
     *              Otherwise, ignore
     * \param pContext: pass context from CConnect::sigBlockShowWidget()
     *
     * \~
     * \see CConnect::sigBlockShowWidget()
     */
    virtual void slotBlockShowWidget(const QString& className, int &nRet, void* pContext);
    /*!
     * \~chinese
     * 阻塞后台线程，并在前台线程中显示消息对话框(QMessageBox)
     *
     * \~english
     * \brief Block background threads and display message dialogs in foreground threads (QMessageBox)
     * 
     * \~
     * \see CConnect::sigBlockShowMessageBox
     */
    virtual void slotBlockShowMessageBox(const QString& szTitle,
                                         const QString& szMessage,
                                         QMessageBox::StandardButtons buttons,
                                         QMessageBox::StandardButton& nRet,
                                         bool &checkBox,
                                         QString szCheckBoxContext = QString());
    /*!
     * \brief Block background threads and display input dialogs in foreground threads (QInputDialog)
     * \~
     * \see CConnecter::sigBlockInputDialog() SetConnecter
     */
    virtual void slotBlockInputDialog(const QString& szTitle,
                                      const QString& szLable,
                                      const QString& szMessage,
                                      QString& szText
                                      );

private Q_SLOTS:
    //! \~chinese \note 仅由 CBackendDesktop::SetConnect() 使用
    //! \~english \note The slot only is used by CBackendDesktop::SetConnect()
    virtual void slotSetClipboard(QMimeData *data);
private:
Q_SIGNALS:
    void sigClipBoardChanged();

protected:
    QMenu m_Menu;
    QAction* m_pActionSettings;
private Q_SLOTS:
    virtual void slotSettings();

private:
    CPlugin* m_pPlugin;
    CStats m_Stats;
};
