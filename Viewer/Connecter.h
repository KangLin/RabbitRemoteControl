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
#include "viewer_export.h"

class CPluginViewer;
class CConnect;

/*!
 * \~chinese
 * \brief 描述连接应用接口。
 * \note 此类是用户接使用接口，由插件实现
 * \details 
 *  已经提供以下类型的基本实现：
 *  1. 桌面类连接： \ref CConnecterDesktop
 *  2. 控制台类连接：\ref CConnecterPluginsTerminal
 *  
 * \~english
 * \brief Connecter interface
 * \note  The class is a interface used by Use UI
 * \details Basic implementations of the following types have been provided:
 *   1. Desktop type: \ref CConnecterDesktop
 *   2. Termianal type: \ref CConnecterPluginsTerminal
 * 
 * \~
 * \see   CPluginViewer CFrmViewer CConnect
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

    const CPluginViewer* GetPluginFactory() const;
    
    /*!
     * \~chinese \brief 从文件中加载参数
     * \~english \brief Load parameters from file
     */
    virtual int Load(QDataStream& d) = 0;
    /*!
     * \~chinese \brief 保存参数到文件中
     * \~english Save parameters to file
     */
    virtual int Save(QDataStream& d) = 0;
    
public Q_SLOTS:
    /**
     * \~chinese
     * \brief 开始连接
     * \note 由用户调用，插件不能直接调用此函数。
     *       插件连接好后，触发信号 sigConnected()
     *       
     * \~english
     * \brief Start connect
     * \note Call by user, The plugin don't call it. 
     *       When plugin is connected, it emit sigConnected()
     */
    virtual int Connect() = 0;
    /*!
     * \~chinese
     * \brief 关闭连接
     * \note 由用户调用，插件不能直接调用此函数。
     *       插件断开连接后，触发信号 sigDisconnected()
     *       
     * \~english
     * \brief Close connect
     * \note Call by user, The plugin don't call it.
     *       When plugin is disconnected, it emit sigDisconnected()
     */
    virtual int DisConnect() = 0;
    
    // \~chinese 下列槽仅由插件使用
    // \~english Follow slot only is used by plugin
    virtual void slotSetClipboard(QMimeData *data);
    virtual void slotSetServerName(const QString &szName);
    
Q_SIGNALS:
    void sigConnected();
    void sigDisconnected();

    /// \~chinese \note 在插件中使用时，请使用 slotSetServerName
    /// \~english \note Please use slotSetServerName, when is useed in plugin
    void sigUpdateName(const QString& szName);

    void sigError(const int nError, const QString &szError);
    void sigInformation(const QString& szInfo);
    
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
     */
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) = 0;

private:
    const CPluginViewer* m_pPluginFactory;

    QString m_szServerName;
};

#endif // CCONNECTER_H
