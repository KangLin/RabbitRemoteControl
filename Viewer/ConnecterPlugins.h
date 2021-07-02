// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERBACKTHREAD_H
#define CCONNECTERBACKTHREAD_H

#pragma once

#include "Connect.h"
#include "Parameter.h"
#include "FrmViewer.h"

class CConnectThread;

/**
 * \~chinese 
 * \brief 它实现一个远程桌面后台线程处理连接
 * \note 此接口仅由插件实现
 * 
 * \~english
 * \brief it is implement a background connect thread of the remote desktop.
 * \note The interface only is implemented by plugin
 * 
 * \~
 * \see CConnectThread
 */
class RABBITREMOTECONTROL_EXPORT CConnecterPlugins : public CConnecter
{
    Q_OBJECT
public:
    explicit CConnecterPlugins(CPluginFactory *parent = nullptr);
    virtual ~CConnecterPlugins();
    
    /**
     * \return \li >=0 Success
     *         \li < 0 fail
     */
    virtual int OnRun();
    /// \~chinese 实例化 CConnect 对象
    /// \~english Instance connect
    virtual CConnect* InstanceConnect() = 0;
    
    virtual const QString Name();
    virtual QWidget* GetViewer() override;
    virtual QString ServerName() override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;
    virtual int OpenDialogSettings(QWidget *parent) override;

public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

protected:
    virtual int OnConnect();
    virtual int OnDisConnect();
    virtual int OnLoad(QDataStream& d);
    virtual int OnSave(QDataStream& d);
    /**
     * \~chinese 得到设置对话框
     * \param parent: 返回窗口的父窗口
     * \return QDialog*: 插件实现时，此对话框必须设置属性 Qt::WA_DeleteOnClose，
     *                   它的所有者是调用者
     * 
     * \~english
     * \brief GetDialogSettings
     * \param parent: the parent windows of the dialog of return
     * \return QDialog*: then QDialog must set attribute Qt::WA_DeleteOnClose;
     *         The ownership is caller.
     */
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) = 0;

private:
    bool m_bExit;
    CConnectThread* m_pThread;
    CFrmViewer *m_pView;

protected:
    CParameter* m_pParameter;
};

#endif // CCONNECTERBACKTHREAD_H
