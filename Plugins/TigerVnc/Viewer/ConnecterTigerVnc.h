// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERTIGERVNC_H
#define CCONNECTERTIGERVNC_H

#include "ConnecterDesktop.h"
#include "DlgSettingsTigerVnc.h"
#include "ParameterTigerVnc.h"

class CConnecterTigerVnc : public CConnecterDesktop
{
    Q_OBJECT

public:
    explicit CConnecterTigerVnc(CPluginViewer *parent);
    virtual ~CConnecterTigerVnc() override;

    virtual qint16 Version() override;
    virtual QString ServerName() override;

protected:
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) override;

    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

    virtual CConnect* InstanceConnect() override;

public:
    CParameterTigerVnc m_Para;
    CConnect* m_pConnect;

public Q_SLOTS:
    virtual int Connect() override;
    virtual int DisConnect() override;

Q_SIGNALS:
    void sigConnect(CConnecter*);
    void sigDisconnect(CConnecter*);
};

#endif // CCONNECTERTIGERVNC_H
