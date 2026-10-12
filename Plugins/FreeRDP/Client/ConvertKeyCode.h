//! Author: Kang Lin <kl222@126.com>

#pragma once

#include <QKeyEvent>

class CConvertKeyCode
{
public:
    CConvertKeyCode();
    
    static quint32 QtToScanCode(int key, Qt::KeyboardModifiers modifiers);
};
