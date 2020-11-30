#ifndef CCONNECTFREERDP_H
#define CCONNECTFREERDP_H

#include "Connect.h"
#include "freerdp/freerdp.h"

class CConnectFreeRdp;

struct ClientContext{
    rdpContext* pContext;
    CConnectFreeRdp* pConnect;
};

class CConnectFreeRdp : public CConnect
{
    Q_OBJECT

public:
    explicit CConnectFreeRdp(CFrmViewer* pView = nullptr,
                             QObject *parent = nullptr);
    virtual ~CConnectFreeRdp() override;
    
    static BOOL Client_global_init();
    static void Client_global_uninit();
    static BOOL Client_new(freerdp* instance, rdpContext* context);
    static void Client_free(freerdp* instance, rdpContext* context);
    static int Client_start(rdpContext* context);
    static int Client_stop(rdpContext* context);
    
    // CConnect interface
public Q_SLOTS:
    virtual int Connect() override;
    virtual int Disconnect() override;
    virtual int Process() override;
    virtual void slotClipBoardChange() override;
    
private:
    int RdpClientEntry(RDP_CLIENT_ENTRY_POINTS* pEntryPoints);
    
private:
    ClientContext* m_pContext;
	rdpSettings* m_pSettings;
	RDP_CLIENT_ENTRY_POINTS m_ClientEntryPoints;
};

#endif // CCONNECTFREERDP_H
