// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QAction>
#include "Operate.h"

class CBackend;
class CBackendThread;
class CFrmViewServer;

/*!
 * \brief The COperateServer class
 * \see CFrmViewServer
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
