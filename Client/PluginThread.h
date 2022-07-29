// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINHREAD_H_KL_2021_08_05
#define CPLUGINHREAD_H_KL_2021_08_05

#pragma once

#include "ConnectThread.h"
#include "ConnecterDesktop.h"

/*!
 * \~chinese 一个线程处理多个连接者 CConnecterDesktop 。
 *           一个连接者对应一个连接，也就是一个线程处理多个连接。
 *           此类仅用于 CPluginClientThread
 *           由插件使用。例如：CPluginTigerVnc
 * \~english One thread handles multiple CConnecterDesktop.
 *           only used by CPluginClientThread.
 *           Be used by plugin. eg: CPluginTigerVnc
 * \~
 * \see CPluginClientThread CManageConnect CConnecterDesktop
 * \ingroup LIBAPI_THREAD
 */
class CLIENT_EXPORT CPluginThread : public QThread
{
    Q_OBJECT
    
public:
    explicit CPluginThread();
    virtual ~CPluginThread();

Q_SIGNALS:
    void sigConnect(CConnecterDesktop*);
    void sigDisconnect(CConnecterDesktop*);

protected:
    virtual void run() override;
};

#endif // CPLUGINHREAD_H_KL_2021_08_05
