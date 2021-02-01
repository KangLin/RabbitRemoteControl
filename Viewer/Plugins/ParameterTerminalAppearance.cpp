#include "ParameterTerminalAppearance.h"

CParameterTerminalAppearance::CParameterTerminalAppearance()
{
    colorScheme = "GreenOnBlack";
    cursorShape = Konsole::Emulation::KeyboardCursorShape::BlockCursor;
    scrollBarPosition = QTermWidget::NoScrollBar;
    termTransparency = 0;
    flowControl = true;
#if defined (Q_OS_LINUX)
    szKeyBindings = "linux";
#endif
    textCodec = "UTF-8";
}

QDataStream &operator<<(QDataStream &data, const CParameterTerminalAppearance &para)
{
    data << (CParameter&)para
         << para.font
         << para.colorScheme
         << para.termTransparency
         << para.flowControl
         << para.backgroupImage
            ;
    
    data << (int)para.cursorShape
         << (int)para.scrollBarPosition
         ;
    return data;
}

QDataStream &operator>>(QDataStream &data, CParameterTerminalAppearance &para)
{
    data >> (CParameter&)para
         >> para.font
         >> para.colorScheme
         >> para.termTransparency
         >> para.flowControl
         >> para.backgroupImage
         ;
    
    int cursorShape = 0, scrollBarPosition = 0;
    data >> cursorShape >> scrollBarPosition;
    para.cursorShape = (Konsole::Emulation::KeyboardCursorShape)cursorShape;
    para.scrollBarPosition = (QTermWidget::ScrollBarPosition)scrollBarPosition;
    return data;
}
