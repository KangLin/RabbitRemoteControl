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
    explicit CConnecterTigerVnc(CPluginClient *parent);
    virtual ~CConnecterTigerVnc() override;

    virtual const QString Id() override;
    virtual qint16 Version() override;

protected:
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) override;
    virtual CConnect* InstanceConnect() override;
    virtual QString ServerName() override;

private:
    CParameterTigerVnc m_Para;
};

#endif // CCONNECTERTIGERVNC_H
