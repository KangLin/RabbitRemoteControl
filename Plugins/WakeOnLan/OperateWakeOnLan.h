// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Operate.h"
#include "FrmWakeOnLan.h"
#include "Arp.h"

class COperateWakeOnLan : public COperate
{
    Q_OBJECT

public:
    explicit COperateWakeOnLan(CPlugin *plugin);
    virtual ~COperateWakeOnLan();

    // CConnecter interface
public:
    [[nodiscard]] virtual const QString Id() override;
    [[nodiscard]] virtual const QString Name() override;
    [[nodiscard]] virtual const qint16 Version() const override;
    [[nodiscard]] virtual QWidget *GetViewer() override;

protected:
    virtual int Initial() override;
    virtual int Clean() override;

    // CConnecter interface
public slots:
    virtual int Start() override;
    virtual int Stop() override;

Q_SIGNALS:
    void sigGetMac(CParameterWakeOnLan* p);
    void sigWakeOnLan(CParameterWakeOnLan* p);

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;
    virtual int SetGlobalParameters(CParameterPlugin* pPara) override;

    CFrmWakeOnLan* m_pView;
    CWakeOnLanModel* m_pModel;
    CArp m_Arp;
    CParameterPlugin* m_pParameterPlugin;

private Q_SLOTS:
    void slotAdd();

    // CConnecter interface
protected:
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;
};
