#ifndef CPARAMETERTERMINALAPPEARANCE_H
#define CPARAMETERTERMINALAPPEARANCE_H

#include "ParameterConnecter.h"
#include "qtermwidget.h"
#include <QFont>
#include "terminal_export.h"

class TERMINAL_EXPORT CParameterTerminal : public CParameterConnecter
{
    Q_OBJECT
    
public:
    CParameterTerminal();
    
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

    const QFont &GetFont() const;
    void SetFont(const QFont &newFont);
    bool GetSizeHint() const;
    void SetSizeHint(bool newSizeHint);
    const QString &GetColorScheme() const;
    void SetColorScheme(const QString &newColorScheme);
    Konsole::Emulation::KeyboardCursorShape GetCursorShape() const;
    void SetCursorShape(Konsole::Emulation::KeyboardCursorShape newCursorShape);
    QTermWidget::ScrollBarPosition GetScrollBarPosition() const;
    void SetScrollBarPosition(QTermWidget::ScrollBarPosition newScrollBarPosition);
    int GetTransparency() const;
    void SetTransparency(int newTermTransparency);
    const QString &GetBackgroupImage() const;
    void SetBackgroupImage(const QString &newBackgroupImage);
    bool GetFlowControl() const;
    void SetFlowControl(bool newFlowControl);
    bool GetFlowControlWarning() const;
    void SetFlowControlWarning(bool newFlowControlWarning);
    bool GetMotionAfterPasting() const;
    void SetMotionAfterPasting(bool newMotionAfterPasting);
    bool GetDisableBracketedPasteMode() const;
    void SetDisableBracketedPasteMode(bool newDisableBracketedPasteMode);
    bool GetDirectional() const;
    void SetDirectional(bool newBiDirectional);
    const QString &GetKeyBindings() const;
    void SetKeyBindings(const QString &newSzKeyBindings);
    const QString &GetTextCodec() const;
    void SetTextCodec(const QString &newTextCodec);
    int GetHistorySize() const;
    void SetHistorySize(int newHistorySize);
    
private:
    QFont m_Font;
    bool m_bSizeHint;       // show size when windows resize
    QString m_szColorScheme;
    Konsole::Emulation::KeyboardCursorShape m_CursorShape;
    QTermWidget::ScrollBarPosition m_ScrollBarPosition;
    int m_nTransparency;
    QString m_BackgroupImage;

    bool m_bFlowControl;
    bool m_bFlowControlWarning;
    
    bool m_bMotionAfterPasting;
    bool m_bDisableBracketedPasteMode;
    bool m_bDirectional;
    QString m_szKeyBindings;
    QString m_szTextCodec;
    int m_nHistorySize;
};

#endif // CPARAMETERTERMINALAPPEARANCE_H
