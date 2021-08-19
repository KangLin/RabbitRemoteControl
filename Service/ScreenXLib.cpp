#include "ScreenXLib.h"
#include "RabbitCommonLog.h"
#include <X11/Xlib.h>
#include <stdexcept>

CScreen* CScreen::Instance()
{
    static CScreen* p = nullptr;
    if(!p)
    {
        p = new CScreenXLib();
    }
    return p;
}

CScreenXLib::CScreenXLib(QObject *parent) : CScreen(parent)
{
    QString szErr;
    Display* dsp = NULL;
    dsp = XOpenDisplay(NULL);/* Connect to a local display */
    if(NULL == dsp)
    {
        LOG_MODEL_ERROR("CScreenXLib", "Cannot connect to local display");
        throw std::runtime_error("Cannot connect to local display");
    }
    do{
        Visual* vis = DefaultVisual(dsp, DefaultScreen(dsp));
        if (vis->c_class != TrueColor) {
            LOG_MODEL_ERROR("CScreenXLib", "pseudocolour not supported");
            szErr = "pseudocolour not supported";
            break;
        }
        
        m_pImage = XCreateImage(dsp, vis, DefaultDepth(dsp, DefaultScreen(dsp)),
                                ZPixmap, 0, 0, Width(), Height(),
                                BitmapPad(dsp), 0);
        m_pImage->data = (char *)malloc(m_pImage->bytes_per_line * m_pImage->height);
        if (m_pImage->data == NULL) {
            LOG_MODEL_ERROR("CScreenXLib", "malloc() failed");
            szErr = "malloc() failed";
        }
    } while(0);
    
    XCloseDisplay(dsp);
    if(!szErr.isEmpty())
        throw std::runtime_error(szErr.toStdString().c_str());
    
    m_Screen = QImage((uchar*)m_pImage->data, Width(), Height(), GetFormat(m_pImage));
}

CScreenXLib::~CScreenXLib()
{
    if(m_pImage)
        XDestroyImage(m_pImage);
}

int CScreenXLib::Width()
{
    int w = 0;
    Display* dsp = NULL;
    dsp = XOpenDisplay(NULL);/* Connect to a local display */
    if(NULL == dsp)
    {
        LOG_MODEL_ERROR("CScreenXLib", "Cannot connect to local display");
        return 0;
    }
    w = DisplayWidth(dsp, DefaultScreen(dsp));
    XCloseDisplay(dsp);
    return w;
}

int CScreenXLib::Height()
{
    int h = 0;
    Display* dsp = NULL;
    dsp = XOpenDisplay(NULL);/* Connect to a local display */
    if(NULL == dsp)
    {
        LOG_MODEL_ERROR("CScreenXLib", "Cannot connect to local display");
        return 0;
    }
    h = DisplayHeight(dsp, DefaultScreen(dsp));
    XCloseDisplay(dsp);
    return h;
}

int CScreenXLib::VirtualTop()
{
    int n = 0;
    return n;
}

int CScreenXLib::VirtualLeft()
{
    int n = 0;
    return n;
}

int CScreenXLib::VirtualWidth()
{
    int n = 0;
    return n;
}

int CScreenXLib::VirtualHeight()
{
    int n = 0;
    return n;
}

int CScreenXLib::VisibleMonitorCount()
{
    int n = 0;
    return n;
}

void myImageCleanupHandler(void *info)
{
    LOG_MODEL_DEBUG("CScreenXLib", "void myImageCleanupHandler(void *info)");
    XDestroyImage(static_cast<XImage*>(info));
}

QImage::Format CScreenXLib::GetFormat(XImage *img)
{
    switch(img->bits_per_pixel)
    {
    case 32:
        return QImage::Format_ARGB32;
    case 24:
        return QImage::Format_RGB888;
    default:
        LOG_MODEL_ERROR("CScreenXLib", "Don't support format:%d", img->bits_per_pixel);
        return QImage::Format_Invalid;
    }

    return QImage::Format_Invalid;
}

QImage CScreenXLib::GetScreen(int index)
{
    Window desktop = 0;
    Display* dsp = NULL;
    
    int screen_width;
    int screen_height;
    
    dsp = XOpenDisplay(NULL);/* Connect to a local display */
    if(NULL == dsp)
    {
        LOG_MODEL_ERROR("CScreenXLib","Cannot connect to local display");
        return m_Screen;
    }
    do{
        desktop = DefaultRootWindow(dsp); // RootWindow(dsp,0);/* Refer to the root window */
        if(0 == desktop)
        {
            LOG_MODEL_ERROR("CScreenXLib", "cannot get root window");
            break;
        }
        
        /* Retrive the width and the height of the screen */
        screen_width = Width(); // DisplayWidth(dsp, 0);
        screen_height = Height(); // DisplayHeight(dsp, 0);
        
        XGetSubImage(dsp, desktop, 0, 0, Width(), Height(), AllPlanes, ZPixmap,
                     m_pImage, 0, 0);
        
        /* Get the Image of the root window */
//        img = XGetImage(dsp, desktop, 0, 0, screen_width, screen_height, ~0, ZPixmap);
//        m_Screen = QImage((uchar*)img->data,
//                          img->width, img->height,
//                          GetFormat(img),
//                          myImageCleanupHandler, img);
    }while(0);
    XCloseDisplay(dsp);
    return m_Screen;
}
