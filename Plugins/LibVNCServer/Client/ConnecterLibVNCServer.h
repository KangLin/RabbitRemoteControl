#ifndef CCONNECTERLIBVNC_H
#define CCONNECTERLIBVNC_H

#include "ConnecterDesktopThread.h"
#include "ConnectLibVNCServer.h"

class CDlgSettingsLibVnc;
class CConnecterLibVNCServer : public CConnecterDesktopThread
{
    Q_OBJECT
public:
    explicit CConnecterLibVNCServer(CPluginClient *parent);
    virtual ~CConnecterLibVNCServer() override;

public:
    virtual qint16 Version() override;

protected:
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual CConnect* InstanceConnect() override;
    
private:
    CParameterLibVNCServer m_Para;
    
    friend CConnectLibVNCServer;
    friend CDlgSettingsLibVnc;
};

#endif // CCONNECTERLIBVNC_H
