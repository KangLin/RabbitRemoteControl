// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QAction>
#include "Operate.h"

class CBackend;
class CBackendThread;
class CFrmViewServer;

/*!
 * \~chinese
 * \defgroup gOperateServer 服务类
 * \~english
 * \defgroup gOperateServer Server class
 * \~
 * \ingroup PLUGIN_API
 */

/*!
 * \~chinese 服务操作接口
 * \note 此接口仅由插件实现。 \n
 *      此接口实例运行在主线程(UI线程)中。 \n
 *      具体的插件需要实现下面接口：
 *         1. 实现 InstanceBackend() ，生成连接对象。连接对象运行在后台线程中。
 *
 * \~english Server operate interface
 * \note The interface only is implemented by plug-in \n
 *     The specific plug-in needs to implement the following interface.
 *         1. Implement InstanceConnect() . Generate a connection object.
 *            The connection object runs in a background thread.
 *
 * \~
 * \see CFrmViewServer CBackendServer
 * \ingroup gOperateServer
 */
class PLUGIN_EXPORT COperateServer : public COperate
{
    Q_OBJECT
public:
    explicit COperateServer(CPlugin *plugin);
    virtual ~COperateServer();

public:
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;

protected:
    /*!
     * \~chinese
     * 新建后端实例。它的所有者是调用者，
     * 如果调用者不再使用它，调用者必须负责释放它。
     *
     * \~english New CBackend. the ownership is caller.
     *        if don't use, the caller must delete it.
     * \~see CBackendThread
     */
    Q_INVOKABLE virtual CBackend* InstanceBackend() = 0;
    Q_INVOKABLE virtual int Initial() override;
    Q_INVOKABLE virtual int Clean() override;

private Q_SLOTS:
    void slotStart(bool checked);

private:
    CFrmViewServer* m_pViewer;
    CBackendThread* m_pThread;

    QAction* m_pActionStart;
};
