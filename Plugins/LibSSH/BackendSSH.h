// Author: Kang Lin <kl222@126.com>

#pragma once
#include "libssh/callbacks.h"
#include "libssh/libssh.h"

#include "Backend.h"
#include "OperateSSH.h"
#include "ChannelSSHTerminal.h"

class CBackendSSH : public CBackend
{
    Q_OBJECT
public:
    explicit CBackendSSH(COperateSSH* pOperate);
    virtual ~CBackendSSH();

protected:
    virtual OnInitReturnValue OnInit() override;
    virtual int OnProcess() override;
    virtual int OnClean() override;

private:
    COperateSSH* m_pOperate;
    CParameterTerminalBase* m_pPara;
    CChannelSSHTerminal* m_pChannelSSH;
    QTermWidget* m_pTerminal;
};
