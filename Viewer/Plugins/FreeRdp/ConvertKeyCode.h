#ifndef CCONVERTKEYCODE_H
#define CCONVERTKEYCODE_H

#include "freerdp/scancode.h"

class CConvertKeyCode
{
public:
    CConvertKeyCode();
    
    static UINT32 QtToScanCode(int key);
};

#endif // CCONVERTKEYCODE_H
