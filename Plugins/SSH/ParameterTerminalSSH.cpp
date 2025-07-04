// Author: Kang Lin <kl222@126.com>

#include "ParameterTerminalSSH.h"

CParameterTerminalSSH::CParameterTerminalSSH(CParameterOperate *parent, const QString &szPrefix)
    : CParameterTerminalBase{parent, szPrefix}
    , m_SSH(this)
{}
