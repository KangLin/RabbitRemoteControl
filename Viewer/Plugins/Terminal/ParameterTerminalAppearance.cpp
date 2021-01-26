#include "ParameterTerminalAppearance.h"

CParameterTerminalAppearance::CParameterTerminalAppearance()
{
    cursorShape = Konsole::Emulation::KeyboardCursorShape::BlockCursor;
    scrollBarPosition = QTermWidget::NoScrollBar;
    termTransparency = 0;
    flowControl = true;
}
