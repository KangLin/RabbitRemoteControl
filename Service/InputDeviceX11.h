#ifndef CINPUTDEVICEX11_H
#define CINPUTDEVICEX11_H

#include "InputDevice.h"
#include <X11/XKBlib.h>

class CInputDeviceX11 : public CInputDevice
{
public:
    explicit CInputDeviceX11();
    virtual ~CInputDeviceX11();
    
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

#endif // CINPUTDEVICEX11_H
