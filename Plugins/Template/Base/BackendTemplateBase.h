// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Backend.h"
#include "OperateTemplateBase.h"

class CBackendTemplateBase : public CBackend
{
    Q_OBJECT
public:
    explicit CBackendTemplateBase(COperateTemplateBase *pOperate = nullptr, bool bStopSignal = true);
    virtual ~CBackendTemplateBase();

    // CBackend interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;
};
