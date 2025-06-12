// Author: Kang Lin <kl222@126.com>

#ifndef CCHANNELSSHTUNNEL_H
#define CCHANNELSSHTUNNEL_H

#pragma once

#include <QSharedPointer>
#include <QSocketNotifier>
#include <QMutex>

#include "libssh/libssh.h"
#include "libssh/callbacks.h"

#include "Channel.h"
#include "ParameterSSHTunnel.h"
#include "Event.h"
#include "Backend.h"

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
class PLUGIN_EXPORT CChannelSSHTunnel : public CChannel
{
    Q_OBJECT

public:
    explicit CChannelSSHTunnel(CParameterSSHTunnel* parameter,
                               CParameterNet* remote,
                               CBackend *pBackend,
                               bool bWakeUp = true,
                               QObject *parent = nullptr);
    virtual ~CChannelSSHTunnel();

    // QIODevice interface
public:
    virtual bool open(OpenMode mode) override;
    virtual void close() override;
    
    virtual QString GetDetails() override;
    int Process();
    virtual int WakeUp();
    virtual int DoWait(bool bWrite, int timeout);
    int GetSocket();

    // QIODevice interface
protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

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

protected:
    ssh_session m_Session;
    ssh_channel m_Channel;

private:
    CBackend *m_pBackend;
    ssh_pcap_file m_pcapFile;
    CParameterSSHTunnel* m_pParameter;
    CParameterNet* m_pRemoteNet;
    /*
    QSocketNotifier* m_pSocketRead;
    QSocketNotifier* m_pSocketWrite;
    QSocketNotifier* m_pSocketException;
    */
    Channel::CEvent* m_pEvent;
};

#endif // CCHANNELSSHTUNNEL_H
