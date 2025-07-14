// Author: Kang Lin <kl222@126.com>

#pragma once

#include "libssh/libssh.h"
#include "libssh/callbacks.h"
#include "Channel.h"
#include "ParameterSSH.h"
#include "Backend.h"
#include "plugin_export.h"
#include "Event.h"

class PLUGIN_EXPORT CChannelSSH : public CChannel
{
    Q_OBJECT
public:
    explicit CChannelSSH(CBackend *pBackend, CParameterSSH* pPara,
                         bool bWakeUp = true, QObject *parent = nullptr);
    virtual ~CChannelSSH();

public:
    virtual bool open(OpenMode mode) override;
    virtual void close() override;
    virtual int WakeUp();

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
    Channel::CEvent* m_pEvent;

private:
    ssh_pcap_file m_pcapFile;
};
