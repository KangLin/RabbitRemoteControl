// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINHREAD_H_KL_2021_08_05
#define CPLUGINHREAD_H_KL_2021_08_05

#pragma once

#include <QThread>
#include "ConnecterConnect.h"

/*!
 * \~chinese 一个线程处理多个连接者 CConnecterConnect 。
 *           一个连接者对应一个连接，也就是一个线程处理多个连接。
 *           此类仅用于 CPluginClientThread
 *           由插件使用。
 * \~english One thread handles multiple CConnecterConnect.
 *           only used by CPluginClientThread.
 *           Be used by plugin.
 * \~
 * \see CPluginClientThread CManageConnect CConnecterConnect
 * \ingroup LIBAPI_THREAD
 */
class CLIENT_EXPORT CPluginThread : public QThread
{
    Q_OBJECT
    
public:
    explicit CPluginThread(QObject *parent = nullptr);
    virtual ~CPluginThread();

Q_SIGNALS:
    void sigConnect(CConnecterConnect*);
    void sigDisconnect(CConnecterConnect*);

protected:
    virtual void run() override;
};

#endif // CPLUGINHREAD_H_KL_2021_08_05
