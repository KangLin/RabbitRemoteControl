//! @author: Kang Lin <kl222@126.com>

#ifndef CCONVERTKEYCODE_H
#define CCONVERTKEYCODE_H

#include "freerdp/scancode.h"
#include <QKeyEvent>

class CConvertKeyCode
{
public:
    CConvertKeyCode();
    
    static UINT32 QtToScanCode(int key, QKeyEvent *e);
};

#endif // CCONVERTKEYCODE_H
