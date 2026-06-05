// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QThread>
#include "Operate.h"

/*!
 * \~chinese 一个线程处理多个操作 COperate 。
 *           此类仅用于 CPluginBackendThread
 *           由插件使用。
 * \~english One thread handles multiple operate.
 *           only used by CPluginBackendThread.
 *           Be used by plugin.
 * \~
 * \see CPluginBackendThread CManageBackend
 * \ingroup LIBAPI_THREAD
 */
class CPluginThread : public QThread
{
    Q_OBJECT
    
public:
    explicit CPluginThread(QObject *parent = nullptr);
    virtual ~CPluginThread();

Q_SIGNALS:
    void sigNewBackend(COperate* pOperate);
    void sigDeleteBackend(COperate* pOperate);

protected:
    virtual void run() override;
};
