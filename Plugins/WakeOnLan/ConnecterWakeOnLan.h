// Author: Kang Lin <kl222@126.com>

#ifndef CONNECTERWAKEONLAN_H
#define CONNECTERWAKEONLAN_H

#pragma once

#include "ConnecterConnect.h"
#include "FrmWakeOnLan.h"


class CConnectWakeOnLan;
class CConnecterWakeOnLan : public CConnecterConnect
{
    Q_OBJECT

public:
    explicit CConnecterWakeOnLan(CPluginClient *plugin);
    virtual ~CConnecterWakeOnLan();

    // CConnecter interface
public:
    virtual const QString Id() override;
    virtual const QString Name() override;
    virtual qint16 Version() override;
    virtual QWidget *GetViewer() override;
    virtual int OnInitial() override;
    virtual int OnClean() override;

    // CConnecter interface
public slots:
    virtual int Connect() override;
    virtual int DisConnect() override;

    // CConnecterConnect interface
public:
    virtual CConnect *InstanceConnect() override;

Q_SIGNALS:
    void sigGetMac(CParameterWakeOnLan* p);
    void sigWakeOnLan(CParameterWakeOnLan* p);

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;
    virtual int SetParameterClient(CParameterClient* pPara) override;

    CFrmWakeOnLan* m_pView;
    CWakeOnLanModel* m_pModel;
    CConnectWakeOnLan* m_pConnect;
    CParameterClient* m_pParameterClient;

private Q_SLOTS:
    void slotAdd();

    // CConnecter interface
protected:
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;
};

#endif // CONNECTERWAKEONLAN_H
