#include "InputDevice.h"
#include "RabbitCommonLog.h"
#include <X11/Xlib.h>

CInputDevice::CInputDevice(QObject *parent) : QObject(parent)
{
}

CInputDevice::~CInputDevice()
{
    LOG_MODEL_DEBUG("InputDevcie", "CInputDevice::~CInputDevice()");
}

int CInputDevice::KeyEvent(quint32 keysym, quint32 keycode, bool down)
{
    return 0;
}

// Simulate mouse click
void
click (Display *display, int button, bool press)
{
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
}

// Get mouse coordinates
void
coords (Display *display, int *x, int *y)
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
void
move (Display *display, int x, int y)
{
    XWarpPointer (display, None, None, 0,0,0,0, x, y);
    XFlush (display);
    
}

// Move mouse pointer (absolute)
void
move_to (Display *display, int x, int y)
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

int CInputDevice::MouseEvent(MouseButtons buttons, QPoint pos)
{
    Display *display = XOpenDisplay(NULL);
    if(display == NULL)
    {
        LOG_MODEL_ERROR("CInputDevice", "Open display fail");
        return -1;
    }
    Window root = DefaultRootWindow(display);
    
    // - Has the pointer moved since the last event?
    if (m_LastPostion != pos)
        move_to(display, pos.x(), pos.y());
    
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
    
    // Check the right button on chanage state
    if((m_LastButtons & RightButton) != (RightButton & buttons))
    {
        if(buttons & RightButton)
            click(display, Button3, true);
        else
            click(display, Button3, false);
    }
    
    XCloseDisplay(display);
    return 0;
}

int CInputDevice::MouseEvent(MouseButtons buttons, int x, int y)
{
    return MouseEvent(buttons, QPoint(x, y));
}
