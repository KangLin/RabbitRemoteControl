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
#include "viewer_export.h"

class CPluginViewer;
class CConnect;
class CParameter;
class CManageConnecter;

/*!
 * \~chinese
 * \brief 描述连接应用接口。
 * \note 此类是用户接使用接口，由插件实现
 * \details 
 *  已经提供以下类型的基本实现：
 *  1. 桌面类连接： \ref CConnecterDesktop
 *  2. 控制台类连接：\ref CConnecterTerminal
 *  
 * \~english
 * \brief Connecter interface
 * \note  The class is a interface used by Use UI
 * \details Basic implementations of the following types have been provided:
 *   1. Desktop type: \ref CConnecterDesktop
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
     * \~english
     * \param parent: The parent pointer must be specified as
     *        the corresponding CPluginViewer derived class
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
     * \brief 当前连接名。例如：服务名或 IP:端口
     * \return 返回服务名
     * 
     * \~english
     * \brief Current connect server name. eg: Server name or Ip:Port 
     * \return Current connect server name.
     */
    virtual QString ServerName();
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
     * \~english Save parameters to file
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
     *       插件断开连接后，触发信号 sigDisconnected()
     *       
     * \~english
     * \brief Close connect
     * \note Only call by user, The plugin don't call it.
     *       When plugin is disconnected, it emit sigDisconnected()
     * 
     * \~
     * \see sigDisconnected()
     */
    virtual int DisConnect() = 0;
    
    // \~chinese 下列槽仅由插件使用
    // \~english Follow slot only is used by plugin
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
     * \~chinese \note 仅在插件中使用时，请使用 slotSetServerName
     * \~english \note When only used in the plug-in, please use slotSetServerName
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

    virtual CParameter* GetParameter();
    virtual int SetParameter(CParameter* p);

private:
    const CPluginViewer* m_pPluginViewer;
    friend CManageConnecter;
    QString m_szServerName;
    
    // The owner is a derived class of this class
    CParameter* m_pParameter;
};

#endif // CCONNECTER_H
