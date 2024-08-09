// Author: Kang Lin <kl222@126.com>

#ifndef CHANNELSSHTUNNELFORWARD_H
#define CHANNELSSHTUNNELFORWARD_H

#include "ChannelSSHTunnel.h"

/*!
 * \~chinese 包括 SSH 隧道和用于转发数据的本地 SOCKET 服务。
 * \~english Includes SSH tunneling and a local socket service for forwarding data.
 * \~
 * \note It is only used in CSSHTunnelThread
 * \see CSSHTunnelThread
 * \ingroup LIBAPI_CHANNEL
 */
class CChannelSSHTunnelForward : public CChannelSSHTunnel
{
    Q_OBJECT
public:
    explicit CChannelSSHTunnelForward(QSharedPointer<CParameterChannelSSH> parameter,
                                     QObject *parent = nullptr);
    virtual ~CChannelSSHTunnelForward();
    
    // QIODevice interface
public:
    virtual bool open(OpenMode mode) override;
    virtual void close() override;
    virtual int Process();
    
Q_SIGNALS:
    void sigServer(quint16 nPort);

private:
    socket_t m_Server;
    socket_t m_Connector;
    
    int CloseSocket(socket_t &s);
    int AcceptConnect();
    int ReadConnect();
    int SSHReadyRead();
    
    char* m_pBuffer;
    const int m_BufferLength = 1024;
};

#endif // CHANNELSSHTUNNELFORWARD_H
