#ifndef CCONNECTERLIBVNC_H
#define CCONNECTERLIBVNC_H

#include "ConnectThreadLibVnc.h"
#include "ConnectLibVnc.h"

class CConnecterLibVnc : public CConnecter
{
public:
    CConnecterLibVnc(QObject *parent = nullptr);
    virtual ~CConnecterLibVnc() override;
    
    // CConnecter interface
public:
    virtual QString ServerName() override;
    virtual QString Name() override;
    virtual QString Description() override;
    virtual QString Protol() override;
    virtual qint16 Version() override;
    virtual QDialog *GetDialogSettings(QWidget *parent) override;
    virtual int Load(QDataStream &d) override;
    virtual int Save(QDataStream &d) override;
    
public slots:
    virtual int Connect() override;
    virtual int DisConnect() override;
    virtual void slotSetClipboard(QMimeData *data) override;
    
private:
    CConnectThreadLibVnc* m_pThread;
    
    CConnectLibVnc::strPara m_Para;
    friend CConnectLibVnc;
};

#endif // CCONNECTERLIBVNC_H
