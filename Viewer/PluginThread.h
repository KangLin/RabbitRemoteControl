#ifndef CPLUGINHREAD_H_KL_2021_08_05
#define CPLUGINHREAD_H_KL_2021_08_05

#pragma once

#include "ConnectThread.h"
#include "Connecter.h"

/*!
 * \~chinese 一个线程处理多个连接者 CConnecter 。
 *           一个连接者对应一个连接，也就是一个线程处理多个连接。
 *           由插件使用。例如：CPluginTigerVnc
 * \~english One thread handles multiple CConnecter.
 *           Be used by plugin. eg: CPluginTigerVnc
 * \~
 * \see CManageConnect CConnecter
 * \ingroup LIBAPI_THREAD
 */
class VIEWER_EXPORT CPluginThread : public CConnectThread
{
    Q_OBJECT
    
public:
    explicit CPluginThread();
    virtual ~CPluginThread();

Q_SIGNALS:
    void sigConnect(CConnecter*);
    void sigDisconnect(CConnecter*);

protected:
    virtual void run() override;
};

#endif // CPLUGINHREAD_H_KL_2021_08_05
