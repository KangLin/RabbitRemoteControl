// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERTIGERVNC_H
#define CCONNECTERTIGERVNC_H

#include "ConnecterDesktop.h"
#include "DlgSettingsTigerVnc.h"

class CConnecterTigerVnc : public CConnecterDesktop
{
    Q_OBJECT

public:
    explicit CConnecterTigerVnc(CPluginViewer *parent);
    virtual ~CConnecterTigerVnc() override;

    virtual qint16 Version() override;

protected:
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) override;

    virtual int OnLoad(QDataStream& d) override;
    virtual int OnSave(QDataStream& d) override;

    virtual CConnect* InstanceConnect() override;

public:
    CConnectTigerVnc::strPara m_Para;
    CConnect* m_pConnect;

public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

Q_SIGNALS:
    void sigConnect(CConnecter*);
    void sigDisconnect(CConnecter*);
};

#endif // CCONNECTERTIGERVNC_H
