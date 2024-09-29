// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERFREERDP_H
#define CCONNECTERFREERDP_H

#include "Connecter.h"
#include "ConnecterThread.h"
#include "ParameterFreeRDP.h"

#include <QLoggingCategory>

class CConnecterFreeRDP : public CConnecterThread
{
    Q_OBJECT
public:
    explicit CConnecterFreeRDP(CPluginClient *plugin);
    virtual ~CConnecterFreeRDP() override;

public:
    virtual qint16 Version() override;
    
protected:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

    virtual CConnect *InstanceConnect() override;

private:
    //! [Initialize parameter]
    CParameterFreeRDP m_ParameterFreeRdp;
    //! [Initialize parameter]
    QLoggingCategory m_Logger;
};

#endif // CCONNECTERFREERDP_H
