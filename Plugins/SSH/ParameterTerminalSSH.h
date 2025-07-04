// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterTerminalBase.h"

class CParameterTerminalSSH : public CParameterTerminalBase
{
    Q_OBJECT
public:
    explicit CParameterTerminalSSH(CParameterOperate *parent = nullptr,
                                   const QString& szPrefix = QString());
    
    CParameterSSH m_SSH;
};
