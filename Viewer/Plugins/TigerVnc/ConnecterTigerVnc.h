#ifndef CCONNECTERTIGERVNC_H
#define CCONNECTERTIGERVNC_H

#include "ConnectTigerVnc.h"
#include "ConnectThread.h"
#include "FrmViewer.h"
#include "DlgSettings.h"

class CConnecterTigerVnc : public CConnecter
{
    Q_OBJECT
    
public:
    explicit CConnecterTigerVnc(QObject *parent = nullptr);
    virtual ~CConnecterTigerVnc() override;
    
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) override;
    virtual CFrmViewer* GetViewer() override;
    
public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

public:
    strPara m_Para;
    
private:
    CFrmViewer *m_pView;
    CConnectThread* m_pThread;
};

#endif // CCONNECTERTIGERVNC_H
