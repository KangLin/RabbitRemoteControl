// Author: Kang Lin <kl222@126.com>

#ifndef CCONNECTERFREERDP_H
#define CCONNECTERFREERDP_H

#include "ConnecterThread.h"
#include "ParameterFreeRDP.h"

#include "RAILManageWindows.h"

class CConnecterFreeRDP : public CConnecterThread
{
    Q_OBJECT
public:
    explicit CConnecterFreeRDP(CPluginClient *plugin);
    virtual ~CConnecterFreeRDP() override;

    CRAILManageWindows m_RailManageWindows;

public:
    virtual qint16 Version() override;

protected:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;
    virtual CConnect *InstanceConnect() override;
    virtual int Initial() override;
    virtual int Clean() override;

private:
    //! [Initialize parameter]
    CParameterFreeRDP m_ParameterFreeRdp;
    //! [Initialize parameter]
};

#endif // CCONNECTERFREERDP_H
