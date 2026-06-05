// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Backend.h"
#include "OperateTemplateQtEvent.h"

class CBackendTemplateQtEvent : public CBackend
{
    Q_OBJECT
public:
    explicit CBackendTemplateQtEvent(COperateTemplateQtEvent *pOperate = nullptr, bool bStopSignal = true);
    virtual ~CBackendTemplateQtEvent();

    // CBackend interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
};
