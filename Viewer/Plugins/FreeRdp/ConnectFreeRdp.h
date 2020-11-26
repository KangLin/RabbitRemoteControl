#ifndef CCONNECTFREERDP_H
#define CCONNECTFREERDP_H

#include "Connect.h"

class CConnectFreeRdp : public CConnect
{
    Q_OBJECT

public:
    explicit CConnectFreeRdp(CFrmViewer* pView = nullptr,
                             QObject *parent = nullptr);
    virtual ~CConnectFreeRdp() override;
    
    // CConnect interface
public Q_SLOTS:
    virtual int Connect() override;
    virtual int Disconnect() override;
    virtual int Process() override;
    virtual void slotClipBoardChange() override;
};

#endif // CCONNECTFREERDP_H
