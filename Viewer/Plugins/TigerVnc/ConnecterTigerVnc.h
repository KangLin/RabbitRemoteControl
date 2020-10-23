#ifndef CCONNECTERTIGERVNC_H
#define CCONNECTERTIGERVNC_H

#include "Connecter.h"
#include "ConnectTigerVnc.h"
#include "ConnectThread.h"
#include "FrmViewer.h"

class CConnecterTigerVnc : public CConnecter
{
    Q_OBJECT
    
public:
    explicit CConnecterTigerVnc(QObject *parent = nullptr);

    virtual QWidget* GetDialogSettings() override;
    virtual CFrmViewer* GetViewer() override;
    virtual int Connect() override;
    virtual int DisConnect() override;

private:
    CFrmViewer *m_pView;
    CConnectThread* m_pThread;

};

#endif // CCONNECTERTIGERVNC_H
