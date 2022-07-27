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

#include "viewer_export.h"

class CPluginViewer;
class CConnect;
class CParameterConnect;
class CParameterViewer;
class CManagePlugin;

/*!
 * \~chinese
 * \brief 描述连接应用接口。
 * \note 此类是用户使用接口，由插件实现
 * \details 
 * 序列图：\image html docs/Image/PluginViewerSequenceDiagram.svg
 *  已经提供以下类型的基本实现：
 *  1. 桌面类连接：  
 *     1.1. 用于连接是阻塞模型(一个线程处理一个连接)： \ref CConnecterDesktopThread  
 *     1.2. 用于连接是非阻塞模型(一个线程处理多个连接)： \ref CConnecterDesktop  
 *  2. 控制台类连接：\ref CConnecterTerminal
 *  
 * \~english
 * \brief Connecter interface
 * \note  The class is a interface used by Use UI.
 *        It is implemented by the protol plugin.
 * \details
 * Sequen diagram: \image html docs/Image/PluginViewerSequenceDiagram.svg
 * Basic implementations of the following types have been provided:     
 *   1. Desktop type:  
 *      1.1. The connection used is the blocking model
 *           (One thread handles one connection): \ref CConnecterDesktopThread  
 *      1.2. The connection is a non-blocking model
 *           (One thread handles multiple connections): \ref CConnecterDesktop  
 *   2. Termianal type: \ref CConnecterTerminal
 * 
 * \~
 * \see   CPluginViewer CFrmViewer
 * \ingroup VIEWER_API VIEWER_PLUGIN_API
 */
class VIEWER_EXPORT CConnecter : public QObject
{
    Q_OBJECT
    
public:
    /*!
     * \~chinese
     * \param parent: 此指针必须是相应的 CPluginViewer 派生类的实例指针
     * \note 如果参数(CParameterConnect 或其派生类）需要 CParameterViewer。
     *       请在其派生类的构造函数中实例化参数，并调用 SetParameter 设置参数指针。
     *       如果参数不需要 CParameterViewer，那请在其派生类重载 SetParameterViewer 。
     * \~english
     * \param parent: The parent pointer must be specified as
     *        the corresponding CPluginViewer derived class
     * \note If the parameters(CParameterConnect or its derived class) requires a CParameterViewer.
     *       Please instantiate the parameters and call SetParameter in the derived class to set the parameters pointer.
     *       If you are sure the parameter does not need CParameterViewer.
     *       please overload the SetParameterViewer in the derived class. don't set it.
     * \~
     * \see CManagePlugin::CreateConnecter SetParameterViewer SetParameter
     *      CParameterConnect CParameterViewer
     */
    explicit CConnecter(CPluginViewer *parent);
    virtual ~CConnecter();
    
    virtual const QString Id();
    virtual const QString Name();
    virtual const QString Description();
    virtual const QString Protol() const;
    virtual qint16 Version() = 0;
    virtual const QIcon Icon() const;

    /*!
     * \~chinese
     * \brief 得到显示视图
     * \return CFrmViewer*: 视图指针。它的所有者是本类的实例
     * 
     * \~english
     * \brief Get Viewer 
     * \return CFrmViewer*: the ownership is a instance of this class
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
    
    //! \~chinese 下列槽仅由插件使用
    //! \~english Follow slot only is used by plugin
    virtual void slotSetClipboard(QMimeData *data);
    virtual void slotSetServerName(const QString &szName);
    
Q_SIGNALS:
    /*!
     * \~chinese 连接成功信号。仅由插件触发
     * \~english Successful connection signal. Only triggered by plugins
     */
    void sigConnected();
    /*!
     * \~chinese 断开连接成功信号。仅由插件触发
     * \~english Successful disconnection signal. Only triggered by plugins
     */
    void sigDisconnected();
    /*!
     * \~chinese \note 名称更新。此信号仅由本类触发
     * \~english \note The name is changed. This signal is only triggered by this class 
     */
    void sigUpdateName(const QString& szName);
    void sigError(const int nError, const QString &szError);
    /*!
     * \brief Show infomation in main windows 
     * \param szInfo
     * \see MainWindow::slotInformation()
     */
    void sigInformation(const QString& szInfo);

    /*!
     * \brief Update parameters, notify application to save or show parameters.
     * \param pConnecter
     */
    void sigUpdateParamters(CConnecter* pConnecter);

protected:
    /*!
     * \~chinese
     * \brief 得到设置对话框
     * \param parent: 返回窗口的父窗口
     * \return QDialog*: 插件实现时，此对话框必须设置属性 Qt::WA_DeleteOnClose，
     *             它的所有者是调用者
     *
     * \~english
     * \brief GetDialogSettings
     * \param parent: the parent windows of the dialog of return
     * \return QDialog*: then QDialog must set attribute Qt::WA_DeleteOnClose;
     *         The ownership is caller.
     *         
     * \~
     * \see OpenDialogSettings
     */
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) = 0;

    virtual int Load(QSettings &set) = 0;
    virtual int Save(QSettings &set) = 0;

    virtual CParameterConnect* GetParameter();
    /*!
     * \brief Set parameter pointer.
     *        Ability to overload this function to set the parameter signal.
     * \param p
     */
    virtual int SetParameter(CParameterConnect* p);

    /*!
     * \~chinese
     * \brief 当前连接名。例如：服务名或 IP:端口
     * \return 返回服务名
     * 
     * \~english
     * \brief Current connect server name. eg: Server name or Ip:Port 
     * \return Current connect server name.
     */
    virtual QString ServerName();

    static QObject* createObject(const QString &className, QObject* parent = NULL);

private Q_SLOTS:
    void slotShowServerName();
    void slotUpdateName();

    /*!
     * \~chinese
     * 从后台线程中阻塞显示窗口
     *
     * \~english
     * \brief When a background thread blocks the display window
     * \param className: show windows class name
     *        The class must have foller public functions:
     *            Q_INVOKABLE void SetContext(void* pContext);
     *            Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);
     * \param nRet: If className is QDialog derived class, QDialog::exec() return value.
     *              Otherwise, ignore
     * \param pContext: pass context from CConnect::sigBlockShowWidget()
     * \~
     * \see CConnect::sigBlockShowWidget()
     */
    virtual void slotBlockShowWidget(const QString& className, int &nRet, void* pContext);
    /*!
     * \~chinese
     * 从后台线程中显示 QMessageBox
     *
     * \~english
     * \brief The background thread uses QMessageBox to block the display window
     * \param title
     * \param message
     * \param buttons
     * \param nRet
     * \param checkBox
     * \param szCheckBoxContext
     */
    virtual void slotBlockShowMessage(QString title, QString message,
                                      QMessageBox::StandardButtons buttons,
                                      QMessageBox::StandardButton& nRet,
                                      bool &checkBox,
                                      QString szCheckBoxContext = QString());

protected:
    const CPluginViewer* m_pPluginViewer;
    /*!
     * \note If CParameterConnect isn't need CParameterViewer.
     *       please overload this function.
     * \see CManagePlugin::CreateConnecter CParameterConnect CParameterViewer
     */
    virtual int SetParameterViewer(CParameterViewer* pPara);

private:
    friend CManagePlugin;
    QString m_szServerName;

    // The owner is a derived class of this class
    CParameterConnect* m_pParameter;
};

#endif // CCONNECTER_H
