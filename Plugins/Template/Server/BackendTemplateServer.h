// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Backend.h"
#include "OperateTemplateServer.h"

class CBackendTemplateServer : public CBackend
{
    Q_OBJECT
public:
    explicit CBackendTemplateServer(COperateTemplateServer *pOperate = nullptr, bool bStopSignal = true);
    virtual ~CBackendTemplateServer();

    // CBackend interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
};
