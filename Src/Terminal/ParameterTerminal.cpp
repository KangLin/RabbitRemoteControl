#include "ParameterTerminal.h"
#include <QApplication>
#include <QDebug>
#include <QSettings>

CParameterTerminal::CParameterTerminal()
{
    m_Font = QApplication::font();
#ifdef Q_OS_MACOS
    font.setFamily(QStringLiteral("Monaco"));
#elif defined(Q_WS_QWS)
    font.setFamily(QStringLiteral("fixed"));
#else
    m_Font.setFamily(QStringLiteral("Monospace"));
#endif
    m_Font.setStyleHint(QFont::TypeWriter);
    m_Font.setPointSize(12);
    
    m_bSizeHint = false;
    m_szColorScheme = "GreenOnBlack";
    m_CursorShape = Konsole::Emulation::KeyboardCursorShape::BlockCursor;
    m_ScrollBarPosition = QTermWidget::ScrollBarRight;
    m_nTransparency = 0;
    m_bFlowControl = false;
    m_bFlowControlWarning = false;
    m_bMotionAfterPasting = false;
    m_bDisableBracketedPasteMode = false;
    m_bDirectional = true;
    
#if defined (Q_OS_LINUX)
    m_szKeyBindings = "linux";
#else
    szKeyBindings = "default";
#endif
    m_szTextCodec = "UTF-8";
    m_nHistorySize = 1000;
}

int CParameterTerminal::Load(QSettings &set)
{
    m_Font.fromString(set.value("Terminal/Font", m_Font.toString()).toString());
    SetSizeHint(set.value("Terminal/SizeHint", GetSizeHint()).toBool());
    m_szColorScheme = set.value("Terminal/ColorScheme", m_szColorScheme).toString();
    m_nTransparency = set.value("Terminal/Transparency",
                                 m_nTransparency).toInt();
    m_bFlowControl = set.value("Terminal/flowControl", m_bFlowControl).toBool();
    m_bFlowControlWarning = set.value("Terminal/flowControlWarning",
                                   m_bFlowControlWarning).toBool();
    m_bMotionAfterPasting = set.value("Terminal/motionAfterPasting",
                                   m_bMotionAfterPasting).toBool();
    m_bDirectional = set.value("Terminal/Directional", m_bDirectional).toBool();
    m_bDisableBracketedPasteMode = set.value("Terminal/disableBracketedPasteMode",
                                          m_bDisableBracketedPasteMode).toBool();
    m_BackgroupImage = set.value("Terminal/BackgroupImage",
                               m_BackgroupImage).toString();
    m_CursorShape = static_cast<Konsole::Emulation::KeyboardCursorShape>(
                set.value("Terminal/cursorShape",static_cast<int>(m_CursorShape)).toInt());
    m_ScrollBarPosition = static_cast<QTermWidget::ScrollBarPosition>
            (set.value("Terminal/scrollBarPosition",
                       static_cast<int>(m_ScrollBarPosition)).toInt());
    m_szKeyBindings = set.value("Terminal/KeyBindings", m_szKeyBindings).toString();
    m_szTextCodec = set.value("Terminal/TextCodec", m_szTextCodec).toString();
    m_nHistorySize = set.value("Terminal/HistorySize", m_nHistorySize).toInt();
    return CParameterConnecter::Load(set);
}

int CParameterTerminal::Save(QSettings &set)
{
    set.setValue("Terminal/Font", m_Font.toString());
    set.setValue("Terminal/SizeHint", GetSizeHint());
    set.setValue("Terminal/ColorScheme", m_szColorScheme);
    set.setValue("Terminal/Transparency", m_nTransparency);
    set.setValue("Terminal/flowControl", m_bFlowControl);
    set.setValue("Terminal/flowControlWarning", m_bFlowControlWarning);
    set.setValue("Terminal/motionAfterPasting", m_bMotionAfterPasting);
    set.setValue("Terminal/Directional", m_bDirectional);
    set.setValue("Terminal/disableBracketedPasteMode", m_bDisableBracketedPasteMode);
    set.setValue("Terminal/BackgroupImage", m_BackgroupImage);
    set.setValue("Terminal/cursorShape", static_cast<int>(m_CursorShape));
    set.setValue("Terminal/scrollBarPosition",
                 static_cast<int>(m_ScrollBarPosition));
    set.setValue("Terminal/KeyBindings", m_szKeyBindings);
    set.setValue("Terminal/TextCodec", m_szTextCodec);
    set.setValue("Terminal/HistorySize", m_nHistorySize);
    return CParameterConnecter::Save(set);
}

int CParameterTerminal::GetHistorySize() const
{
    return m_nHistorySize;
}

void CParameterTerminal::SetHistorySize(int newHistorySize)
{
    if(m_nHistorySize == newHistorySize)
        return;
    m_nHistorySize = newHistorySize;
    SetModified(true);
}

const QString &CParameterTerminal::GetTextCodec() const
{
    return m_szTextCodec;
}

void CParameterTerminal::SetTextCodec(const QString &newTextCodec)
{
    if(m_szTextCodec == newTextCodec)
        return;
    m_szTextCodec = newTextCodec;
    SetModified(true);
}

const QString &CParameterTerminal::GetKeyBindings() const
{
    return m_szKeyBindings;
}

void CParameterTerminal::SetKeyBindings(const QString &newSzKeyBindings)
{
    if(m_szKeyBindings == newSzKeyBindings)
        return;
    m_szKeyBindings = newSzKeyBindings;
    SetModified(true);
}

bool CParameterTerminal::GetDirectional() const
{
    return m_bDirectional;
}

void CParameterTerminal::SetDirectional(bool newBiDirectional)
{
    if(m_bDirectional == newBiDirectional)
        return;
    m_bDirectional = newBiDirectional;
    SetModified(true);
}

bool CParameterTerminal::GetDisableBracketedPasteMode() const
{
    return m_bDisableBracketedPasteMode;
}

void CParameterTerminal::SetDisableBracketedPasteMode(bool newDisableBracketedPasteMode)
{
    if(m_bDisableBracketedPasteMode == newDisableBracketedPasteMode)
        return;
    m_bDisableBracketedPasteMode = newDisableBracketedPasteMode;
    SetModified(true);
}

bool CParameterTerminal::GetMotionAfterPasting() const
{
    return m_bMotionAfterPasting;
}

void CParameterTerminal::SetMotionAfterPasting(bool newMotionAfterPasting)
{
    if(m_bMotionAfterPasting == newMotionAfterPasting)
        return;
    m_bMotionAfterPasting = newMotionAfterPasting;
    SetModified(true);
}

bool CParameterTerminal::GetFlowControlWarning() const
{
    return m_bFlowControlWarning;
}

void CParameterTerminal::SetFlowControlWarning(bool newFlowControlWarning)
{
    if(m_bFlowControlWarning == newFlowControlWarning)
        return;
    m_bFlowControlWarning = newFlowControlWarning;
    SetModified(true);
}

bool CParameterTerminal::GetFlowControl() const
{
    return m_bFlowControl;
}

void CParameterTerminal::SetFlowControl(bool newFlowControl)
{
    if(m_bFlowControl == newFlowControl)
        return;
    m_bFlowControl = newFlowControl;
    SetModified(true);
}

const QString &CParameterTerminal::GetBackgroupImage() const
{
    return m_BackgroupImage;
}

void CParameterTerminal::SetBackgroupImage(const QString &newBackgroupImage)
{
    if(m_BackgroupImage == newBackgroupImage)
        return;
    m_BackgroupImage = newBackgroupImage;
    SetModified(true);
}

int CParameterTerminal::GetTransparency() const
{
    return m_nTransparency;
}

void CParameterTerminal::SetTransparency(int newTermTransparency)
{
    if(m_nTransparency == newTermTransparency)
        return;
    m_nTransparency = newTermTransparency;
    SetModified(true);
}

QTermWidget::ScrollBarPosition CParameterTerminal::GetScrollBarPosition() const
{
    return m_ScrollBarPosition;
}

void CParameterTerminal::SetScrollBarPosition(QTermWidget::ScrollBarPosition newScrollBarPosition)
{
    if(m_ScrollBarPosition == newScrollBarPosition)
        return;
    m_ScrollBarPosition = newScrollBarPosition;
    SetModified(true);
}

Konsole::Emulation::KeyboardCursorShape CParameterTerminal::GetCursorShape() const
{
    return m_CursorShape;
}

void CParameterTerminal::SetCursorShape(Konsole::Emulation::KeyboardCursorShape newCursorShape)
{
    if(m_CursorShape == newCursorShape)
        return;
    m_CursorShape = newCursorShape;
    SetModified(true);
}

const QString &CParameterTerminal::GetColorScheme() const
{
    return m_szColorScheme;
}

void CParameterTerminal::SetColorScheme(const QString &newColorScheme)
{
    if(m_szColorScheme == newColorScheme)
        return;
    m_szColorScheme = newColorScheme;
    SetModified(true);
}

bool CParameterTerminal::GetSizeHint() const
{
    return m_bSizeHint;
}

void CParameterTerminal::SetSizeHint(bool newSizeHint)
{
    if(m_bSizeHint == newSizeHint)
        return;
    m_bSizeHint = newSizeHint;
    SetModified(true);
}

const QFont &CParameterTerminal::GetFont() const
{
    return m_Font;
}

void CParameterTerminal::SetFont(const QFont &newFont)
{
    if(m_Font == newFont)
        return;
    m_Font = newFont;
    SetModified(true);
}
