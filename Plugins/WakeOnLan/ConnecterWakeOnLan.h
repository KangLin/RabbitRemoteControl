// Author: Kang Lin <kl222@126.com>

#ifndef CONNECTERWAKEONLAN_H
#define CONNECTERWAKEONLAN_H

#pragma once
#include "ConnecterConnect.h"
#include "ParameterBase.h"
#include "FrmWakeOnLan.h"
#include "ConnectWakeOnLan.h"

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

    // CConnecter interface
public slots:
    virtual int Connect() override;
    virtual int DisConnect() override;
    // CConnecterConnect interface
public:
    virtual CConnect *InstanceConnect() override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

    CParameterBase m_Parameter;
    CFrmWakeOnLan* m_pView;
    CConnectWakeOnLan* m_pConnect;
};

#endif // CONNECTERWAKEONLAN_H
