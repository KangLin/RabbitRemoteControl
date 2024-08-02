// Author: Kang Lin <kl222@126.com>

#ifndef CSSHTUNNELTHREAD_H
#define CSSHTUNNELTHREAD_H

#include <QThread>

#include "ParameterChannelSSH.h"

/*!
 * \brief The CSSHTunnelThread class
 * \~
 * \snippet Plugins/LibVNCServer/Client/ConnectLibVNCServer.cpp Use SSH Tunnel
 * \see CChannelSSHTunnelForward
 * \ingroup LIBAPI_CHANNEL
 */
class CHANNEL_EXPORT CSSHTunnelThread : public QThread
{
    Q_OBJECT
public:
    explicit CSSHTunnelThread(QSharedPointer<CParameterChannelSSH> parameter);
    virtual ~CSSHTunnelThread();
    
    void run() override;

    bool m_bExit;
    
Q_SIGNALS:
    /*!
     * \~chinese 当 SOCKET 服务建立监听后触发，用于通知其它本地 SOCKET 可以连接它
     * \~english It is triggered when the socket service establishes a listener
     *      and is used to notify other local sockets that it can connect to it
     * \~
     * \snippet Plugins/LibVNCServer/Client/ConnectLibVNCServer.cpp connect local socket server
     */
    void sigServer(quint16 nPort);
    void sigError(const int nError, const QString &szError = QString());
    void sigDisconnect();
    
private:
    QSharedPointer<CParameterChannelSSH> m_Parameter;
};

#endif // CSSHTUNNELTHREAD_H
