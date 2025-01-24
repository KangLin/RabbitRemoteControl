// Author: Kang Lin <kl222@126.com>

#ifndef RAILEVENT_H
#define RAILEVENT_H

#include <QEvent>
#include "RAIL.h"

class CRAILEvent : public QEvent
{
public:
    enum RAIL_TYPE {
        Activate = QEvent::User + 2,
        SystemCommand,
    };
    Q_ENUM(RAIL_TYPE)

    explicit CRAILEvent(RAIL_TYPE type);

    UINT32 m_WindowId;
    bool m_bActivate;
    UINT16 m_SystemCommand;
};

#endif // RAILEVENT_H
