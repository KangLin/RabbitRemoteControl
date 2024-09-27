// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERBACKTHREAD_H
#define CCONNECTERBACKTHREAD_H

#pragma once

#include "ConnectDesktop.h"
#include "ParameterBase.h"
#include "FrmViewer.h"

class CConnectThread;

/**
 * \~chinese
 * \brief 默认启动一个后台线程。实现一个后台线程处理一个连接。
 *        可与插件接口从 CPluginClient 派生的插件一起使用，用于连接是阻塞模型的。
 * \details
 * 原理：在 Connect() 中启动一个后台线程 CConnectThread 。
 *      在线程中调用 InstanceConnect() 实例化 CConnect ，
 *      并在 CConnect::Connect() 启动定时器，
 *      此定时器在后台线程中调用，通过对定时器的操作，实现一个非 Qt 事件循环(可能会阻塞)，详见 CConnect 。
 *      并且 CConnect 仍然支持 Qt 事件（QObject 的 信号 － 槽 机制）(非阻塞）。
 *
 * 序列图：\image html docs/Image/PluginClientBlockSequenceDiagram.svg
 *
 * \note 此接口仅由插件实现。 \n
 *      此接口实例运行在主线程(UI线程)中。 \n
 *      具体的插件需要实现下面接口：
 *         1. 实现 InstanceConnect() ，生成连接对象。连接对象运行在后台线程中。
 *
 * \~english
 * \brief It starts a background thread by default.
 *        It implements a background thread to handle a connection.
 *        Can be used with plugins whose plugin interface
 *        derives from CPluginClient for connection is blocking model.
 * \details 
 *     Principle: Start a background thread (CConnectThread) in Connect() .
 *     Call InstanceConnect() in the thread to instantiate CConnect,
 *     and start the timer in CConnect::Connect().
 *     This timer is called in the background thread.
 *     Through the operation of the timer,
 *     start a non-Qt event loop (that is, normal loop processing. May block), See CConnect.
 *     And CConnect supports the Qt event (the signal-slot mechanism of QObject. no-block).
 *
 *     Sequen diagram: \image html docs/Image/PluginClientBlockSequenceDiagram.svg
 *
 * \note The interface only is implemented by plug-in \n
 *     The specific plug-in needs to implement the following interface. 
 *         1. Implement InstanceConnect() . Generate a connection object.
 *            The connection object runs in a background thread.
 *
 * \~
 * \see CConnect CConnectThread CConnecter CPluginClient
 */
class CLIENT_EXPORT CConnecterDesktopThread : public CConnecter
{
    Q_OBJECT

public:
    explicit CConnecterDesktopThread(CPluginClient *parent = nullptr);
    virtual ~CConnecterDesktopThread();

    virtual QWidget* GetViewer() override;

    /*!
     * \~chinese
     * 新建 CConnect 对象。它的所有者是调用者，
     * 如果调用者不再使用它，调用者必须负责释放它。
     * 
     * \~english New connect. the ownership is caller.
     *        if don't use, the caller must delete it.
     */
    virtual CConnect* InstanceConnect() = 0;

    /*!
     * \brief Get parameter
     */
    virtual CParameterBase* GetParameter() override;

public Q_SLOTS:
    /*!
     * \~chinese 启动一个后台线程，并建立 CConnect 实例
     * \~english Start a background thread, and create an instance of CConnect .
     * \~
     * \see CConnectThread
     */
    virtual int Connect() override;
    virtual int DisConnect() override;

protected:
    virtual QString ServerName() override;
    
private:
    /*!
     * \~chinese \brief 加载参数
     * \~english \brief Load parameters
     */
    virtual int Load(QSettings &set) override;
    /*!
     * \~chinese 保存参数
     * \~english Save parameters
     */
    virtual int Save(QSettings &set) override;

private:
    CConnectThread* m_pThread;
    CFrmViewer *m_pView;
};

#endif // CCONNECTERBACKTHREAD_H
