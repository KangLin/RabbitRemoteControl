// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateDesktop.h"
#include "ParameterVnc.h"

class COperateVnc : public COperateDesktop
{
    Q_OBJECT

public:
    explicit COperateVnc(CPlugin *plugin);
    virtual ~COperateVnc() override;

    virtual const QString Id() override;
    virtual const qint16 Version() const override;

protected:
    virtual QDialog* OnOpenDialogSettings(QWidget* parent = nullptr) override;
    virtual CBackend* InstanceBackend() override;
    virtual QString ServerName() override;

private:
    virtual int Initial() override;
    virtual int Clean() override;

private:
    CParameterVnc m_Para;
};

