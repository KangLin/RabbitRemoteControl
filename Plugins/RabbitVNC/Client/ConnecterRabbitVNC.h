// Author: Kang Lin <kl222@126.com>

#ifndef CConnecterRabbitVnc_H
#define CConnecterRabbitVnc_H

#include "ConnecterDesktop.h"
#include "DlgSettingsRabbitVNC.h"
#include "ParameterRabbitVNC.h"

class CConnecterRabbitVNC : public CConnecterDesktop
{
    Q_OBJECT

public:
    explicit CConnecterRabbitVNC(CPluginClient *parent);
    virtual ~CConnecterRabbitVNC() override;

    virtual const QString Id() override;
    virtual qint16 Version() override;

protected:
    virtual QDialog* GetDialogSettings(QWidget* parent = nullptr) override;
    virtual CConnect* InstanceConnect() override;
    virtual QString ServerName() override;

private:
    CParameterRabbitVNC m_Para;
};

#endif // CConnecterRabbitVnc_H
