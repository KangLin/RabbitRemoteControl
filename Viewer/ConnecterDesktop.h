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
 * \brief 它实现一个远程桌面后台线程处理一个远程桌面连接
 * \details
 *   1. 实现 \ref InstanceConnect() ，生成连接对象
 *   2. 实现 \ref GetDialogSettings() ，得到参数对话框
 *   3. 实现 \ref OnLoad(QDataStream& d) 和 \ref OnSave(QDataStream& d)
 * \note 此接口仅由插件实现
 * 
 * \~english
 * \brief it is implement a background connect thread of the remote desktop.
 * \note The interface only is implemented by plugin
 * \details 
 *   1. Implement \ref InstanceConnect()
 *   2. Implement \ref GetDialogSettings
 *   3. Implement \ref OnLoad(QDataStream& d) and \ref OnSave(QDataStream& d)
 *   
 * \~  
 * \see CConnectThread CConnecter
 */
class VIEWER_EXPORT CConnecterDesktop : public CConnecter
{
    Q_OBJECT
public:
    explicit CConnecterDesktop(CPluginViewer *parent = nullptr);
    virtual ~CConnecterDesktop();
    
    virtual const QString Name();
    virtual QWidget* GetViewer() override;
    virtual QString ServerName() override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;
    
    /*
     * \~chinese
     * 新建 CConnect 对象。它的所有者是调用者，
     * 如果调用者不再使用它，调用者必须负责释放它。
     * 
     * \~english New connect. the ownership is caller.
     *        if don't use, the caller must delete it.
     */
    virtual CConnect* InstanceConnect() = 0;

public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

protected:
    virtual int OnLoad(QDataStream& d);
    virtual int OnSave(QDataStream& d);

private:
    CConnectThread* m_pThread;
    CFrmViewer *m_pView;

protected:
    CParameter* m_pParameter;
};

#endif // CCONNECTERBACKTHREAD_H
