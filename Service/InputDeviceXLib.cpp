// Author: Kang Lin <kl222@126.com>

// 参考： https://github.com/KangLin/Documents/blob/master/os/Keyboard.md

#include <QLoggingCategory>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#ifdef HAVE_XTEST
#include <X11/extensions/XTest.h>
#endif
#include "InputDeviceXLib.h"

Q_LOGGING_CATEGORY(LogInput, "Device.Input")

QSharedPointer<CInputDevice> CInputDevice::GenerateObject()
{
    return QSharedPointer<CInputDevice>(new CInputDeviceXLib());
}

CInputDeviceXLib::CInputDeviceXLib() : CInputDevice(),
    rawKeyboard(false)
{
}

CInputDeviceXLib::~CInputDeviceXLib()
{
    qDebug(LogInput) << "CInputDeviceXLib::~CInputDeviceXLib()";
}

#ifdef HAVE_XTEST
KeyCode CInputDeviceXLib::XkbKeysymToKeycode(Display* dpy, KeySym keysym) {
  XkbDescPtr xkb;
  XkbStateRec state;
  unsigned int mods;
  unsigned keycode;

  xkb = XkbGetMap(dpy, XkbAllComponentsMask, XkbUseCoreKbd);
  if (!xkb)
    return 0;

  XkbGetState(dpy, XkbUseCoreKbd, &state);
  // XkbStateFieldFromRec() doesn't work properly because
  // state.lookup_mods isn't properly updated, so we do this manually
  mods = XkbBuildCoreState(XkbStateMods(&state), state.group);

  for (keycode = xkb->min_key_code;
       keycode <= xkb->max_key_code;
       keycode++) {
    KeySym cursym;
    unsigned int out_mods;
    XkbTranslateKeyCode(xkb, keycode, mods, &out_mods, &cursym);
    if (cursym == keysym)
      break;
  }

  if (keycode > xkb->max_key_code)
    keycode = 0;

  XkbFreeKeyboard(xkb, XkbAllComponentsMask, True);

  // Shift+Tab is usually ISO_Left_Tab, but RFB hides this fact. Do
  // another attempt if we failed the initial lookup
  if ((keycode == 0) && (keysym == XK_Tab) && (mods & ShiftMask))
    return XkbKeysymToKeycode(dpy, XK_ISO_Left_Tab);

  return keycode;
}
#endif

int CInputDeviceXLib::KeyEvent(quint32 keysym, quint32 xtcode, bool down)
{
    int nRet = 0;

    int keycode = 0;
    
#ifdef HAVE_XTEST
    Display  *display = XOpenDisplay(NULL);
    do {
    // Use scan code if provided and mapping exists
    if (codeMap && rawKeyboard && xtcode < codeMapLen)
        keycode = codeMap[xtcode];
    
    if (!keycode) {
        if (pressedKeys.find(keysym) != pressedKeys.end())
            keycode = pressedKeys[keysym];
        else {
            // XKeysymToKeycode() doesn't respect state, so we have to use
            // something slightly more complex
            keycode = XkbKeysymToKeycode(display, keysym);
        }
    }

    if (!keycode) {
        qCritical(LogInput) << "Could not map key event to X11 key code";
        nRet = -2;
        break;
    }
    
    if (down)
        pressedKeys[keysym] = keycode;
    else
        pressedKeys.erase(keysym);
    
    qDebug(LogInput, "%d %s", keycode, down ? "down" : "up");
    
    XTestFakeKeyEvent(display, keycode, down, CurrentTime);
    } while(0);
    XCloseDisplay(display);
#endif
    
    return 0;
}

// Simulate mouse click
void click(Display *display, int button, bool press)
{
#ifdef HAVE_XTEST
    XTestFakeButtonEvent(display, button, press, CurrentTime);
#else
    // Create and setting up the event
    XEvent event;
    memset (&event, 0, sizeof (event));
    event.xbutton.button = button;
    event.xbutton.same_screen = True;
    event.xbutton.subwindow = DefaultRootWindow (display);
    while (event.xbutton.subwindow)
    {
        event.xbutton.window = event.xbutton.subwindow;
        XQueryPointer (display, event.xbutton.window,
                       &event.xbutton.root, &event.xbutton.subwindow,
                       &event.xbutton.x_root, &event.xbutton.y_root,
                       &event.xbutton.x, &event.xbutton.y,
                       &event.xbutton.state);
    }
    // Press
    if(press)
    {
        event.type = ButtonPress;
        if (XSendEvent (display, PointerWindow, True, ButtonPressMask, &event) == 0)
            fprintf (stderr, "Error to send the event!\n");
    } else {
        if (XSendEvent (display, PointerWindow, True, ButtonReleaseMask, &event) == 0)
            fprintf (stderr, "Error to send the event!\n");
        XFlush (display);    
    }
    XFlush (display);
#endif
}

// Get mouse coordinates
void coords (Display *display, int *x, int *y)
{
    XEvent event;
    XQueryPointer (display, DefaultRootWindow (display),
                   &event.xbutton.root, &event.xbutton.window,
                   &event.xbutton.x_root, &event.xbutton.y_root,
                   &event.xbutton.x, &event.xbutton.y,
                   &event.xbutton.state);
    *x = event.xbutton.x;
    *y = event.xbutton.y;
}

// Move mouse pointer (relative)
void move (Display *display, int x, int y)
{
    XWarpPointer (display, None, None, 0,0,0,0, x, y);
    XFlush (display);
    
}

// Move mouse pointer (absolute)
void move_to (Display *display, int x, int y)
{
    int cur_x, cur_y;
    coords (display, &cur_x, &cur_y);
    XWarpPointer (display, None, None, 0,0,0,0, -cur_x, -cur_y);
    XWarpPointer (display, None, None, 0,0,0,0, x, y);
}

// Get pixel color at coordinates x,y
//void
//pixel_color (Display *display, int x, int y, XColor *color)
//{
//  XImage *image;
//  image = XGetImage (display, DefaultRootWindow (display), x, y, 1, 1, AllPlanes, XYPixmap);
//  color->pixel = XGetPixel (image, 0, 0);
//  XDestroyImage (image);
//  XQueryColor (display, DefaultColormap(display, DefaultScreen (display)), color);
//}

int CInputDeviceXLib::MouseEvent(MouseButtons buttons, QPoint pos)
{
    Display *display = XOpenDisplay(NULL);
    if(display == NULL)
    {
        qCritical(LogInput) << "Open display fail";
        return -1;
    }
    Window root = DefaultRootWindow(display);
    
    // - Has the pointer moved since the last event?
    if (m_LastPostion != pos)
#ifdef HAVE_XTEST
        XTestFakeMotionEvent(display, DefaultScreen(display),
                             pos.x(),
                             pos.y(),
                             CurrentTime);
#else
        XWarpPointer (display, root, root, 0, 0, 0, 0, pos.x(), pos.y());
#endif
    
    // Check the left button on change state
    if((m_LastButtons & LeftButton) != (LeftButton & buttons))
    {
        if(buttons & LeftButton)
            click(display, Button1, true);
        else
            click(display, Button1, false);
    }
    
    // Check the middle button on change state
    if((m_LastButtons & MiddleButton) != (MiddleButton & buttons))
    {
        if(buttons & MiddleButton)
            click(display, Button2, true);
        else
            click(display, Button2, false);
    }
    
    // Check the right button on change state
    if((m_LastButtons & RightButton) != (RightButton & buttons))
    {
        if(buttons & RightButton)
            click(display, Button3, true);
        else
            click(display, Button3, false);
    }
    m_LastButtons = buttons;
    XCloseDisplay(display);
    return 0;
}

int CInputDeviceXLib::MouseEvent(MouseButtons buttons, int x, int y)
{
    return MouseEvent(buttons, QPoint(x, y));
}
