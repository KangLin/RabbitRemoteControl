// Author: Kang Lin <kl222@126.com>

#ifndef CSSHTUNNELTHREAD_H
#define CSSHTUNNELTHREAD_H

#include <QThread>

#include "ParameterSSHTunnel.h"
#include "Backend.h"

/*!
 * \~chinese 实现通过本地 SOCKET 与 SSH 隧道转发数据。适用于库没有实现传输层接口，只有 socket 的情况。
 * \note 当线程完成后，会自动删除实例对象。
 *
 * \~english Data is forwarded over a local socket and SSH tunnel.
 *  It is suitable for cases where the library
 *  does not implement a transport layer interface, only socket.
 * \note When the thread finished, the thread will be deleted.
 *
 * \~
 * \snippet Plugins/LibVNCServer/Client/ConnectLibVNCServer.cpp Use SSH Tunnel
 * \see CChannelSSHTunnelForward
 * \ingroup LIBAPI_CHANNEL
 */
class CLIENT_EXPORT CSSHTunnelThread : public QThread
{
    Q_OBJECT
public:
    explicit CSSHTunnelThread(CParameterSSHTunnel* parameter,
                              CParameterNet* remote, CBackend* pBackend);
    virtual ~CSSHTunnelThread();
    
    void run() override;
    
    void Exit();
    
Q_SIGNALS:
    /*!
     * \~chinese 当 SOCKET 服务建立监听后触发，用于通知其它本地 SOCKET 可以连接它
     * \~english It is triggered when the socket service establishes a listener
     *      and is used to notify other local sockets that it can connect to it
     * \~
     * \snippet Plugins/LibVNCServer/Client/ConnectLibVNCServer.cpp connect local socket server
     */
    void sigServer(QString szHost, quint16 nPort);
    void sigServer(QString szUnixDomainSocket);
    void sigError(const int nError, const QString &szError = QString());
    void sigDisconnect();
    
private:
    CBackend* m_pBackend;
    CParameterSSHTunnel* m_pParameter;
    CParameterNet* m_pRemoteNet;
    bool m_bExit;
};

#endif // CSSHTUNNELTHREAD_H
