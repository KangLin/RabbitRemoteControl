// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTER_H
#define CCONNECTER_H

#pragma once

#include <QObject>
#include <QDir>
#include <QtPlugin>
#include <QDataStream>
#include <QDialog>
#include <QIcon>
#include <QMimeData>
#include <QSettings>
#include <QMessageBox>
#include <QMenu>

#include "client_export.h"
#include "ParameterBase.h"

class CPluginClient;

/*!
 * \~chinese
 * \brief 连接者应用接口。
 * \note
 *   - 此接口仅由是用户使用。它由插件实现。
 *   - 它的实例在主线程中。
 * \details 
 * 序列图：\image html docs/Image/PluginClientSequenceDiagram.svg
 *  已经提供以下类型的基本实现：\n
 *  1. 桌面类连接： \n
 *     1.1. 用于连接是阻塞模型(一个后台线程处理一个连接)： \ref CConnecterThread \n
 *     1.2. 用于连接是非阻塞模型(一个后台线程处理多个连接)： \ref CConnecterConnect \n
 *  2. 工具类： \n
 *     2.1. 用于连接是非阻塞模型（在主线程）： \ref CConnecterConnect \n
 *  3. 控制台类连接：\ref CConnecterTerminal
 *
 * \~english
 * \brief Connecter interface
 * \note
 *   - This interface is only used by users. It is implemented by plugins.
 *   - Its instance is in the main thread.
 * \details
 * Sequen diagram: \image html docs/Image/PluginClientSequenceDiagram.svg
 * Basic implementations of the following types have been provided: \n
 *   1. Desktop type: \n
 *      1.1. The connection used is the blocking model
 *           (a blackground thread handles one connection): \ref CConnecterThread \n
 *      1.2. The connection is a non-blocking model
 *           (a blackground thread handles multiple connections): \ref CConnecterConnect \n
 *   2. Tool type: \n
 *      2.1. The connection is a non-blocking model(in main thread): \ref CConnecterConnect \n
 *   3. Termianal type: \ref CConnecterTerminal
 *
 * \~
 * \see CPluginClient
 * \ingroup CLIENT_API CLIENT_PLUGIN_API
 */
class CLIENT_EXPORT CConnecter : public QObject
{
    Q_OBJECT
    
public:
    /*!
     * \~chinese
     * \param plugin: 此指针必须是相应的 CPluginClient 派生类的实例指针
     * \note 如果参数( CParameterConnecter 或其派生类）需要 CParameterClient 。
     *       请在其派生类的构造函数中实例化参数，并调用 CConnecter::SetParameter 设置参数指针。
     *       如果参数不需要 CParameterClient ，那请在其派生类重载 CConnecter::SetParameterClient 。
     *
     * \~english
     * \param plugin: The plugin pointer must be specified as
     *        the corresponding CPluginClient derived class
     * \note If the parameters( CParameterConnecter or its derived class) requires a CParameterClient .
     *       Please instantiate the parameters and call CConnecter::SetParameter in the derived class to set the parameters pointer.
     *       If you are sure the parameter does not need CParameterClient.
     *       please overload the CConnecter::SetParameterClient in the derived class. don't set it.
     * \~
     * \see CClient::CreateConnecter SetParameterClient SetParameter
     *      CParameterConnecter CParameterClient
     */
    explicit CConnecter(CPluginClient *plugin);
    virtual ~CConnecter();
    
    //! Identity
    virtual const QString Id();
    /*!
     * \~chinese
     * \brief 显示顺序：
     *        - 用户参数设置的名称
     *        - 如果允许，远程服务名。
     *        - 远程地址
     *
     * \~english
     *  Display order:
     *  - User parameter Name()
     *  - if enable, Server name
     *  - Host and port
     *  
     * \~
     * \see ServerName()
     */
    virtual const QString Name();
    //! Description
    virtual const QString Description();
    //! Protocol
    virtual const QString Protocol() const;
    //! Version
    virtual qint16 Version() = 0;
    //! Icon
    virtual const QIcon Icon() const;

    /*!
     * \~chinese
     * \brief 得到显示视图
     * \return QWidget*: 视图指针。它的所有者是本类或其派生类的实例
     *
     * \~english
     * \brief Get Viewer
     * \return QWidget*: the ownership is a instance of this class or its derivative class
     *
     * \~
     * \see sigConnected CFrmViewer::CFrmViewer
     */
    virtual QWidget* GetViewer() = 0;
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
    virtual int OpenDialogSettings(QWidget* parent = nullptr);
    virtual QMenu* GetMenu(QWidget* parent = nullptr);

    /*!
     * \~chinese \brief 从文件中加载参数
     * \~english \brief Load parameters from file
     */
    virtual int Load(QString szFile = QString());

    /*!
     * \~chinese \brief 保存参数到文件中
     * \param szFile: 文件名。当为空时，使用上次使用过的文件。
     * \~english Accept parameters to file
     * \param szFile: When it is empty, the file used last time is used.
     */
    virtual int Save(QString szFile = QString());

public Q_SLOTS:
    /**
     * \~chinese
     * \brief 开始连接
     * \note 仅由用户调用，插件不能直接调用此函数。
     *       插件连接好后，触发信号 sigConnected()
     *       
     * \~english
     * \brief Start connect
     * \note Only call by user, The plugin don't call it. 
     *       When plugin is connected, it emit sigConnected()
     *       
     * \~
     * \see sigConnected()
     */
    virtual int Connect() = 0;
    /*!
     * \~chinese
     * \brief 关闭连接
     * \note 仅由用户调用，插件不能直接调用此函数。
     *       插件断开连接后，触发信号 sigDisconnected()。调用者收到信号后，删除对象
     *       
     * \~english
     * \brief Close connect
     * \note Only call by user, The plugin don't call it.
     *       When plugin is disconnected, it emit sigDisconnected().
     *       The caller receive the signal, then delete the object.
     * 
     * \~
     * \see sigDisconnected()
     */
    virtual int DisConnect() = 0;

Q_SIGNALS:
    /*!
     * \~chinese 连接成功信号。仅由插件触发。
     *   应用程序需要在此时设置视图属性 Enabled 为 true 。允许接收键盘和鼠标事件
     *
     * \~english Successful connection signal. Triggered only by plugins.
     *   The application needs to set the view property Enabled to true at this point.
     *   enable accept keyboard and mouse event
     *
     * \~
     * \see MainWindow::slotConnected() GetViewer() CFrmViewer::CFrmViewer
     * \snippet App/Client/mainwindow.cpp MainWindow slotConnected
     */
    void sigConnected();
    /*!
     * \~chinese 通知用户断开连接。仅由插件触发。
     *    当从插件中需要要断开连接时触发。例如：对端断开连接、重置连接或者连接出错。
     *    当应用接收到此信号后，调用 DisConnect() 关闭连接。
     * \~english Notify the user to disconnect. Triggered only by plugins
     *    Emit when you need to disconnect from the plug-in.
     *    For example, the peer disconnect or reset the connection
     *    or the connection is error.
     *    When the applicatioin receive the signal,
     *    call DisConnect() to close connect
     */
    void sigDisconnect();
    /*!
     * \~chinese 断开连接成功信号。仅由插件触发
     * \~english Successful disconnection signal. Triggered only by plugins
     * \~
     * \see Disconnect()
     */
    void sigDisconnected();
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
     * \see SetParameter
     */
    void sigUpdateParameters(CConnecter* pConnecter);
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
     * \~chinese 得到参数。它仅由插件使用。所以这里设置为保护成员。
     *           可以在其派生类中重载它为公有函数，以方便插件使用。
     *
     * \~english Get parameter. Used only the plugin. so set protected in here.
     *        Overload it as a public function in the derived class.
     */
    virtual CParameterBase* GetParameter();
    /*!
     * \~chinese 设置参数
     * \note 在派生类的构造函数中先实例化参数，然后在其构造函数中调用此函数设置参数。
     *
     * \~english Set parameters
     * \note  Instantiate a parameter in a derived class constructor,
     *      and then call this function in its constructor to set the parameter.
     *
     * \~
     * \see \ref section_Use_CParameterBase
     * \see SetParameterClient()
     */
    virtual int SetParameter(CParameterBase* p);

private:
    /*!
     * \brief Set CParameterClient
     * \note If CParameterConnecter isn't need CParameterClient.
     *       please overload this function.
     * \see CClient::CreateConnecter CParameterConnecter CParameterClient
     */
    Q_INVOKABLE virtual int SetParameterClient(CParameterClient* pPara);

protected:
    Q_INVOKABLE CPluginClient* GetPlugClient() const;
    /*!
     * \~chinese
     * \brief 当前连接名（远程桌面的名称，如果没有，则是 IP:端口）。例如：服务名或 IP:端口
     * \return 返回服务名
     * 
     * \~english
     * \brief Current connect server name
     *        (remote desktop name, if not present, then IP:PORT).
     *        eg: Server name or Ip:Port 
     * \return Current connect server name.
     */
    virtual QString ServerName();

    static QObject* createObject(const QString &className, QObject* parent = NULL);
    
private:
    /*!
     * \~chinese
     * \brief 得到设置对话框
     * \param parent: 返回窗口的父窗口
     * \return QDialog*: 插件实现时，此对话框必须设置属性 Qt::WA_DeleteOnClose，
     *             它的所有者是调用者
     *
     * \~english
     * \brief Open settgins dialog
     * \param parent: the parent windows of the dialog of return
     * \return QDialog*: then QDialog must set attribute Qt::WA_DeleteOnClose;
     *         The ownership is caller.
     *         
     * \~
     * \see OpenDialogSettings
     */
    virtual QDialog* OnOpenDialogSettings(QWidget* parent = nullptr) = 0;

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
    
private Q_SLOTS:
    //! \~chinese \note 仅由 CConnectDesktop::SetConnecter() 使用
    //! \~english \note The slot only is used by CConnectDesktop::SetConnecter()
    virtual void slotSetServerName(const QString &szName);

    void slotShowServerName();
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
     *            Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);
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
    //! \~chinese \note 仅由 CConnect::SetConnecter() 使用
    //! \~english \note The slot only is used by CConnect::SetConnecter()
    virtual void slotSetClipboard(QMimeData *data);
private:
Q_SIGNALS:
    void sigClipBoardChanged();

private:
    QString m_szServerName;

    CPluginClient* m_pPluginClient;

    // The owner is a derived class of this class
    CParameterBase* m_pParameter;
};

#endif // CCONNECTER_H
