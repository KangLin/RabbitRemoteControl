#ifndef CPARAMETERTERMINALAPPEARANCE_H
#define CPARAMETERTERMINALAPPEARANCE_H

#include "Parameter.h"
#include "qtermwidget.h"
#include <QFont>
#include "terminal_export.h"

class TERMINAL_EXPORT CParameterTerminal : public CParameter
{
    Q_OBJECT
    
public:
    CParameterTerminal();
    
    virtual int OnLoad(const QString& szFile) override;
    virtual int OnSave(const QString& szFile) override;

    QFont font;
    bool sizeHint;       // show size when windows resize
    QString colorScheme;
    Konsole::Emulation::KeyboardCursorShape cursorShape;
    QTermWidget::ScrollBarPosition scrollBarPosition;
    int termTransparency;
    QString backgroupImage;

    bool flowControl;
    bool flowControlWarning;
    
    bool motionAfterPasting;
    bool disableBracketedPasteMode;
    bool biDirectional;
    QString szKeyBindings;
    QString textCodec;
    int historySize;
};

#endif // CPARAMETERTERMINALAPPEARANCE_H
