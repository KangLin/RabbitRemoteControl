#ifndef CCHANNELSSHTUNNEL_H
#define CCHANNELSSHTUNNEL_H

#pragma once

#include <QSharedPointer>
#include <QSocketNotifier>
#include <QMutex>

#include "Channel.h"
#include "libssh/libssh.h"
#include "libssh/callbacks.h"
#include "ParameterChannelSSH.h"
#include "channel_export.h"

/*!
 * \~chinese
 * \brief SSH 隧道
 * 
 * \~english
 * \brief ssh tunnel class
 * 
 * \~
 * \ingroup LIBAPI_CHANNEL
 */
class CHANNEL_EXPORT CChannelSSHTunnel : public CChannel
{
    Q_OBJECT

public:
    explicit CChannelSSHTunnel(QSharedPointer<CParameterChannelSSH> parameter,
                               QObject *parent = nullptr);

    // QIODevice interface
public:
    virtual bool open(OpenMode mode) override;
    virtual void close() override;
    
    int Process();
    int ProcessSocket();

private:
    int verifyKnownhost(ssh_session session);
    int authentication(
        ssh_session session,
        const QString szUser,
        const QString szPassword,
        const QString szPassphrase,
        const int nMethod = SSH_AUTH_METHOD_PASSWORD);
    int authenticationPublicKey(
        ssh_session session,
        const QString szUser,
        const QString szPublicKeyFile,
        const QString szPrivateKeyFile,
        const QString szPassphrase);
    int forward(ssh_session session);

    static void cb_log(ssh_session session,
                       int priority,
                       const char *message,
                       void *userdata);
    
    int writeData();
    
    int InitSemaphore();
    int ReadSemphore();
    int ClearSemphore();
    virtual int WakeUp() override;
   
private:
    ssh_session m_Session;
    ssh_channel m_Channel;
    ssh_pcap_file m_pcapFile;
    QSharedPointer<CParameterChannelSSH> m_Parameter;
    
    QSocketNotifier* m_pSocketRead;
    QSocketNotifier* m_pSocketWrite;
    QSocketNotifier* m_pSocketException;

    int m_eventWriteFD;

};

#endif // CCHANNELSSHTUNNEL_H
