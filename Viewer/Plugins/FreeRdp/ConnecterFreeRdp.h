//! @author: Kang Lin(kl222@126.com)

#ifndef CCONNECTERFREERDP_H
#define CCONNECTERFREERDP_H

#include "Connecter.h"
#include "ConnectThread.h"
#include "freerdp/freerdp.h"

class CConnecterFreeRdp : public CConnecter
{
    Q_OBJECT
public:
    explicit CConnecterFreeRdp(QObject *parent = nullptr);
    virtual ~CConnecterFreeRdp() override;
    
    rdpSettings* m_pSettings;
    
    // CConnecter interface
public:
    virtual QString Name() override;
    virtual QString Description() override;
    virtual QString Protol() override;
    
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;
    
public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;
    
private:
    CConnectThread* m_pThread;
};

#endif // CCONNECTERFREERDP_H
