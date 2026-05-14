// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QSocketNotifier>

#include <libssh/server.h>
#include <libssh/sftpserver.h>
#include <libssh/callbacks.h>

#include "Backend.h"
#include "OperateSftpServer.h"
#include "ParameterSftpServer.h"

class CBackendSftpServer : public CBackend
{
    Q_OBJECT

public:
    explicit CBackendSftpServer(COperateSftpServer *pOperate = nullptr,
                                bool bStopSignal = true);
    virtual ~CBackendSftpServer();

protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;
    virtual int WakeUp() override;

private Q_SLOTS:
    void slotNewConnection();

private:
    CParameterSftpServer* m_pPara;
    ssh_bind m_sshBind;
    ssh_event m_event;
    QSocketNotifier* m_pListenNotifier;

    struct sClientData
    {
        ssh_session session;
        ssh_channel channel;
        sftp_session sftp;
        QTime m_Time;
        int m_nAuthAttempts;
        struct ssh_channel_callbacks_struct channel_cb;
        struct ssh_server_callbacks_struct server_cb;
        CParameterSftpServer* pPara;
    };
    QList<sClientData*> m_Clients;

    static int cbAuthPassword(ssh_session session, const char *user,
                              const char *pass, void *userdata);
    static int cbAuthPublickey(ssh_session session,
                               const char *user,
                               struct ssh_key_struct *pubkey,
                               char signature_state,
                               void *userdata);
    static ssh_channel cbChannelOpen(ssh_session session, void *userdata);

    bool InitHostKey();
    int RemoveClient(sClientData* pClient);
    static void SendBanner(ssh_session session, const QString& user);
};
