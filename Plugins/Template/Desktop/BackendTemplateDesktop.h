// Author: Kang Lin <kl222@126.com>

#pragma once

#include <BackendDesktop.h>
#include <OperateTemplateDesktop.h>

class CBackendTemplateDesktop : public CBackendDesktop
{
    Q_OBJECT
public:
    explicit CBackendTemplateDesktop(COperateTemplateDesktop *pOperate = nullptr);
    virtual ~CBackendTemplateDesktop();

    // CBackend interface
protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;

    // CBackendDesktop interface
public slots:
    virtual void slotClipBoardChanged() override;
};
