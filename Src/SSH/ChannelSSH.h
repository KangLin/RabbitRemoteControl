// Author: Kang Lin <kl222@126.com>

#pragma once

#include "libssh/libssh.h"
#include "libssh/callbacks.h"
#include "Channel.h"
#include "ParameterSSH.h"
#include "Backend.h"
#include "plugin_export.h"

class PLUGIN_EXPORT CChannelSSH : public CChannel
{
    Q_OBJECT
public:
    explicit CChannelSSH(CBackend *pBackend, CParameterSSH* pPara, QObject *parent = nullptr);
    virtual ~CChannelSSH();

    // QIODevice interface
public:
    virtual bool open(OpenMode mode) override;
    virtual void close() override;

protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

    // CChannel interface
public:
    virtual QString GetDetails() override;

private:
    static void cb_log(ssh_session session,
                       int priority,
                       const char *message,
                       void *userdata);
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
    virtual int OnOpen(ssh_session session);
    virtual void OnClose();

protected:
    ssh_session m_Session;
    ssh_channel m_Channel;
    CBackend *m_pBackend;
    CParameterSSH* m_pParameter;

private:
    ssh_pcap_file m_pcapFile;
};
