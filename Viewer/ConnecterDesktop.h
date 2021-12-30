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
 * \brief 它实现一个远程桌面后台线程处理一个远程桌面连接。
 * \details
 * 原理：在 Connect() 中启动一个后台线程 CConnectThread 。
 *      在线程中调用 InstanceConnect() 实例化 CConnect ，
 *      并在 CConnect::Connect() 启动定时器，
 *      此定时器在后台线程中调用，通过对定时器的操作，实现一个非 Qt 事件循环，详见 CConnect 。
 *      并且 CConnect 仍然支持 Qt 事件（QObject 的 信号 － 槽 机制）。
 * \note 此接口仅由插件实现。 \n
 *      具体的插件需要实现下面接口：
 *         1. 实现 InstanceConnect() ，生成连接对象
 *         2. 实现 GetDialogSettings() ，得到参数对话框
 *         3. 实现 Load(QSettings& set) ，加载参数
 *         4. 实现 Save(QSettings& set) ，保存参数
 *   
 * 
 * \~english
 * \brief it is implement a background connect thread of the remote desktop.
 * \details 
 *     Principle: Start a background thread (CConnectThread) in Connect() .
 *     Call InstanceConnect() in the thread to instantiate CConnect,
 *     and start the timer in CConnect::Connect().
 *     This timer is called in the background thread.
 *     Through the operation of the timer,
 *     start a non-Qt event loop (that is, normal loop processing), See CConnect.
 *     And CConnect supports the Qt event (the signal-slot mechanism of QObject) .
 * \note The interface only is implemented by plug-in \n
 *     The specific plug-in needs to implement the following interface. 
 *         1. Implement InstanceConnect()
 *         2. Implement GetDialogSettings
 *         3. Implement Load(QSettings& set)
 *         4. Implement Save(QSettings& set)
 *
 * \~  
 * \see CConnect CConnectThread CConnecter
 */
class VIEWER_EXPORT CConnecterDesktop : public CConnecter
{
    Q_OBJECT
public:
    explicit CConnecterDesktop(CPluginViewer *parent = nullptr);
    virtual ~CConnecterDesktop();

    virtual QWidget* GetViewer() override;
    virtual QString ServerName() override;
    /*!
     * \~chinese \brief 加载参数
     * \~english \brief Load parameters
     */
    virtual int Load(QSettings &set) override;
    /*!
     * \~chinese \brief 保存参数
     * \~english Save parameters
     */
    virtual int Save(QSettings &set) override;
    
    /*!
     * \~chinese
     * 新建 CConnect 对象。它的所有者是调用者，
     * 如果调用者不再使用它，调用者必须负责释放它。
     * 
     * \~english New connect. the ownership is caller.
     *        if don't use, the caller must delete it.
     */
    virtual CConnect* InstanceConnect() = 0;

public Q_SLOTS:
    /*!
     * \~chinese 启动一个后台线程，并建立 CConnect 实例
     * \~english Start a background thread, and create an instance of CConnect .
     * \~
     * \see CConnectThread
     */
    virtual int Connect() override;
    virtual int DisConnect() override;

private:
    CConnectThread* m_pThread;
    CFrmViewer *m_pView;

protected:
};

#endif // CCONNECTERBACKTHREAD_H
