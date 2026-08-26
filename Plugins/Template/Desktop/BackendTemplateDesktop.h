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

    virtual int WakeUp() override;

protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnClean() override;
    virtual int OnProcess() override;

public slots:
    virtual void slotClipBoardChanged() override;

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void InputMethodEvent(QInputMethodEvent *event) override;
};
