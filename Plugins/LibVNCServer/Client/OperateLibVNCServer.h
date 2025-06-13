// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateDesktop.h"
#include "ParameterLibVNCServer.h"

class COperateLibVNCServer : public COperateDesktop
{
    Q_OBJECT
public:
    explicit COperateLibVNCServer(CPlugin *plugin);
    virtual ~COperateLibVNCServer() override;

public:
    virtual const qint16 Version() const override;

protected:
    [[nodiscard]] virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;
    [[nodiscard]] virtual CBackend *InstanceBackend() override;
    virtual int Initial() override;
    virtual int Clean() override;

private:
    CParameterLibVNCServer m_Para;
};
