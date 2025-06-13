// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateDesktop.h"
#include "ParameterFreeRDP.h"

class COperateFreeRDP : public COperateDesktop
{
    Q_OBJECT
public:
    explicit COperateFreeRDP(CPlugin *plugin);
    virtual ~COperateFreeRDP() override;

public:
    [[nodiscard]] virtual const qint16 Version() const override;
protected:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;
    Q_INVOKABLE [[nodiscard]] virtual CBackend *InstanceBackend() override;
    virtual int Initial() override;
    virtual int Clean() override;

private:
    //! [Initialize parameter]
    CParameterFreeRDP m_ParameterFreeRdp;
    //! [Initialize parameter]
};
