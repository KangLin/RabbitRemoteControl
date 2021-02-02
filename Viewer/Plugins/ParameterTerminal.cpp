#include "ParameterTerminal.h"
#include <QApplication>
#include <QDebug>

CParameterTerminal::CParameterTerminal()
{
    font = QApplication::font();
#ifdef Q_OS_MACOS
    font.setFamily(QStringLiteral("Monaco"));
#elif defined(Q_WS_QWS)
    font.setFamily(QStringLiteral("fixed"));
#else
    font.setFamily(QStringLiteral("Monospace"));
#endif
    font.setStyleHint(QFont::TypeWriter);
    font.setPointSize(12);
    
    sizeHint = true;
    colorScheme = "GreenOnBlack";
    cursorShape = Konsole::Emulation::KeyboardCursorShape::BlockCursor;
    scrollBarPosition = QTermWidget::ScrollBarRight;
    termTransparency = 0;
    flowControl = true;
#if defined (Q_OS_LINUX)
    szKeyBindings = "linux";
#else
    szKeyBindings = "default";
#endif
    textCodec = "UTF-8";
    historySize = 1000;
}

QDataStream &operator<<(QDataStream &data, const CParameterTerminal &para)
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
    
    data << para.szKeyBindings
         << para.textCodec
         << para.historySize
            ;
    return data;
}

QDataStream &operator>>(QDataStream &data, CParameterTerminal &para)
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
    
    data >> para.szKeyBindings
         >> para.textCodec
         >> para.historySize
         ;
    
    return data;
}
