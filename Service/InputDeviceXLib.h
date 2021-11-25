#ifndef CINPUTDEVICEXLIB_H
#define CINPUTDEVICEXLIB_H

#pragma once
#include "InputDevice.h"
// Author: Kang Lin <kl222@126.com>

#include <X11/XKBlib.h>

class CInputDeviceXLib : public CInputDevice
{
public:
    explicit CInputDeviceXLib();
    virtual ~CInputDeviceXLib();
    
    virtual int KeyEvent(quint32 keysym, quint32 keycode, bool down = true);
    virtual int MouseEvent(MouseButtons buttons, QPoint pos);
    virtual int MouseEvent(MouseButtons buttons, int x, int y);
    
private:
    KeyCode XkbKeysymToKeycode(Display* dpy, KeySym keysym);
    const unsigned short *codeMap;
    unsigned codeMapLen;
    std::map<KeySym, KeyCode> pressedKeys;
    /*
      Send keyboard events straight through and 
      avoid mapping them to the current keyboard layout
    */
    bool rawKeyboard;
};

#endif // CINPUTDEVICEXLIB_H
