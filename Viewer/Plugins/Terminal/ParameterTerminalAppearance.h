#ifndef CPARAMETERTERMINALAPPEARANCE_H
#define CPARAMETERTERMINALAPPEARANCE_H

#include "Parameter.h"
#include "qtermwidget.h"
#include <QFont>

class CParameterTerminalAppearance : public CParameter
{
public:
    CParameterTerminalAppearance();
    
    QFont font;
    QString colorScheme;
    Konsole::Emulation::KeyboardCursorShape cursorShape;
    QTermWidget::ScrollBarPosition scrollBarPosition;
    int termTransparency;
    bool flowControl;
};

#endif // CPARAMETERTERMINALAPPEARANCE_H
