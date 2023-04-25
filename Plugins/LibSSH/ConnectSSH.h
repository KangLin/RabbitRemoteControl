#ifndef CCONNECTSSH_H
#define CCONNECTSSH_H

#include "Connect.h"
#include "ConnecterSSH.h"
#include "libssh/callbacks.h"
#include "libssh/libssh.h"

class CConnectSSH : public CConnect
{
    Q_OBJECT
public:
    explicit CConnectSSH(CConnecterSSH* pConnecter, QObject *parent = nullptr);
    
    // CConnect interface
protected:
    virtual int SetParameter(void *pPara);
    
public slots:
    virtual int Initialize();
    virtual int OnClean() override;
    virtual int OnInit() override;
    virtual int OnProcess() override;
    virtual void slotClipBoardChanged() override;
    void slotSendData(const char *,int);
    void slotReceivedData(const QString &text);
    
private:
    CConnecterSSH* m_pConnecter;
    CParameterSSH* m_pPara;
    
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
