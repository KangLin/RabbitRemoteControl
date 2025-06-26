#ifndef CCONNECTSSH_H
#define CCONNECTSSH_H

#include "Backend.h"
#include "OperateSSH.h"
#include "libssh/callbacks.h"
#include "libssh/libssh.h"

class CBackendSSH : public CBackend
{
    Q_OBJECT
public:
    explicit CBackendSSH(COperateSSH* pOperate);

protected:
    virtual int SetParameter(void *pPara);
    
public slots:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnProcess() override;
    void slotSendData(const char *,int);
    void slotReceivedData(const QString &text);

private:
    virtual int Initialize();
    virtual int OnClean() override;

private:
    COperateSSH* m_pOperate;
    CParameterTerminalBase* m_pPara;
    
    ssh_session m_pSession;
    ssh_channel m_pChannel;
    ssh_event m_pEvent;
    ssh_connector connector_in, connector_out, connector_err;
    ssh_pcap_file m_pPcapFile;
    
    struct ssh_callbacks_struct *m_pCb;
    
    static int cbAuthCallback(const char *prompt,
                             char *buf,
                             size_t len,
                             int echo,
                             int verify,
                             void *userdata);
    int GetPassword(const char *prompt,
                    char *buf,
                    size_t len,
                    int echo,
                    int verify);
    int VerifyKnownhost(ssh_session session);
    int Authenticate(ssh_session session);
    void error(ssh_session session);
};

#endif // CCONNECTSSH_H
