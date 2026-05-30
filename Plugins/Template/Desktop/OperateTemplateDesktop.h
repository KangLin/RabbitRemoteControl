// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateDesktop.h"

class CParameterTemplateDesktop;
class COperateTemplateDesktop : public COperateDesktop
{
    Q_OBJECT
public:
    COperateTemplateDesktop(CPlugin *plugin);
    virtual ~COperateTemplateDesktop();

private:
    CParameterTemplateDesktop* m_pPara;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

protected:
    virtual CBackend *InstanceBackend() override;
    virtual int Initial() override;
    virtual int Clean() override;
};
