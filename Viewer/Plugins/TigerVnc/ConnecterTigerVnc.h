#ifndef CCONNECTERTIGERVNC_H
#define CCONNECTERTIGERVNC_H

#include "ConnectThread.h"
#include "FrmViewer.h"
#include "DlgSettings.h"

class CConnecterTigerVnc : public CConnecter
{
    Q_OBJECT

public:
    explicit CConnecterTigerVnc(QObject *parent = nullptr);
    virtual ~CConnecterTigerVnc() override;

    virtual QString Name() override;
    virtual QString Description() override;
    virtual QString Protol() override;
    
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) override;
    virtual CFrmViewer* GetViewer() override;

    virtual int Load(QDataStream& d) override;
    virtual int Save(QDataStream& d) override;
    
public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

public:
    CConnectTigerVnc::strPara m_Para;

private:
    CFrmViewer *m_pView;
    CConnectThread* m_pThread;
};

#endif // CCONNECTERTIGERVNC_H
