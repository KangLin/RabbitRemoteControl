// Author: Kang Lin <kl222@126.com>

#pragma once

#include "freerdp/scancode.h"
#include <QKeyEvent>

class CConvertKeyCode
{
public:
    CConvertKeyCode();
    
    static UINT32 QtToScanCode(int key, Qt::KeyboardModifiers modifiers);
};
