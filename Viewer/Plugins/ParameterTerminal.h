#ifndef CPARAMETERTERMINALAPPEARANCE_H
#define CPARAMETERTERMINALAPPEARANCE_H

#include "Parameter.h"
#include "qtermwidget.h"
#include <QFont>

class CParameterTerminal : public CParameter
{
    Q_OBJECT
    
public:
    CParameterTerminal();
    
    QFont font;
    QString colorScheme;
    Konsole::Emulation::KeyboardCursorShape cursorShape;
    QTermWidget::ScrollBarPosition scrollBarPosition;
    int termTransparency;
    QString backgroupImage;

    bool flowControl;
    QString szKeyBindings;
    QString textCodec;
};

QDataStream &operator<<(QDataStream &, const CParameterTerminal &);
QDataStream &operator>>(QDataStream &, CParameterTerminal &);

#endif // CPARAMETERTERMINALAPPEARANCE_H
