//! Author: Kang Lin <kl222@126.com>

#ifndef CONVERTKEYCODE_H
#define CONVERTKEYCODE_H

#include <QObject>

class CConvertKeyCode : public QObject
{
    Q_OBJECT
public:
    explicit CConvertKeyCode(QObject *parent = nullptr);

    static quint32 QtToRfbKey(int inkey, bool modifier);
};

#endif // CONVERTKEYCODE_H
