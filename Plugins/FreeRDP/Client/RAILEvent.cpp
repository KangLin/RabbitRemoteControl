// Author: Kang Lin <kl222@126.com>

#include "RAILEvent.h"

CRAILEvent::CRAILEvent(RAIL_TYPE type)
    : QEvent((Type)type)
    , m_WindowId(0)
    , m_bActivate(false)
    , m_SystemCommand(0)
{}
