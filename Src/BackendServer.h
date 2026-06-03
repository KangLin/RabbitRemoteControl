// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Backend.h"

class COperateServer;

/*!
 * \~chinese
 * \brief 服务接口。它由协议插件实现。
 *
 * \~english
 * \brief Server interface. It is implemented by the Protocol plugin.
 *
 * \~
 * \see COperateServer CFrmViewServer
 * \ingroup gOperateServer
 */
class PLUGIN_EXPORT CBackendServer : public CBackend
{
    Q_OBJECT
public:
    explicit CBackendServer(COperateServer *pOperate = nullptr, bool bStopSignal = true);
    virtual ~CBackendServer();

public Q_SLOTS:
    /*!
     * \brief Disconnect client
     * \param szIp
     * \param port
     */
    virtual void slotDisconnect(const QString& szIp, const quint16 port) = 0;

Q_SIGNALS:
    /*!
     * \brief emit when new client connected
     * \param szIp
     * \param port
     */
    void sigConnected(const QString& szIp, const quint16 port);
    /*!
     * \brief emit when the client disconnected
     * \param szIp
     * \param port
     */
    void sigDisconnected(const QString& szIp, const quint16 port);

private:
    int SetConnect(COperateServer* pOperate);
};
