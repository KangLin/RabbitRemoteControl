#ifndef CSERVICELIBVNCSERVER_H
#define CSERVICELIBVNCSERVER_H

#include "Service.h"
#include <rfb/rfb.h>

class CServiceLibVNCServer : public CService
{
    Q_OBJECT
public:
    explicit CServiceLibVNCServer(CPluginService *plugin);

    bool Enable();

private:
    int OnInit() override;
    virtual int OnClean() override;
    int OnProcess() override;
    rfbScreenInfoPtr m_rfbScreen;
};

#endif // CSERVICELIBVNCSERVER_H
