#ifndef CCHANNELSSHTUNNEL_H
#define CCHANNELSSHTUNNEL_H

#pragma once

#include <Channel.h>
#include <libssh/libssh.h>
#include <libssh/callbacks.h>
#include <QSharedPointer>

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
    
    void run();
protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;
    
private:
    int verifyKnownhost(
        ssh_session session);
    int authenticationUser(ssh_session session,
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

private:
    ssh_session m_Session;
    ssh_channel m_Channel;
    QSharedPointer<CParameterChannelSSH> m_Parameter;
};

#endif // CCHANNELSSHTUNNEL_H
