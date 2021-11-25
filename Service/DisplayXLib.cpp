// Author: Kang Lin <kl222@126.com>

#include "RabbitCommonLog.h"
#include "DisplayXLib.h"

// xlib documents: https://www.x.org/releases/current/doc/libX11/libX11/libX11.html
// X documents: https://www.x.org/releases/current/doc/index.html
// X Window System Concepts: https://www.x.org/wiki/guide/concepts/
// https://www.x.org

CDisplayXLib::CDisplayXLib() : CDisplay(),
    m_pDisplay(NULL),
    m_RootWindow(0),
    m_Format(QImage::Format_Invalid),
    m_pImage(NULL)
{
    Open();
}

CDisplayXLib::~CDisplayXLib()
{
    Close();
}

CDisplay* CDisplay::Instance()
{
    static CDisplay* p = nullptr;
    if(!p) p = new CDisplayXLib();
    return p;
}

int CDisplayXLib::Width()
{
    if(m_pDisplay)
        return DisplayWidth(m_pDisplay, DefaultScreen(m_pDisplay));
    return 0;
}

int CDisplayXLib::Height()
{
    if(m_pDisplay)
        return DisplayHeight(m_pDisplay, DefaultScreen(m_pDisplay));
    return 0;
}

QImage::Format CDisplayXLib::GetFormat()
{
    return m_Format;
}

QImage::Format CDisplayXLib::GetFormat(XImage* pImage)
{
    switch(pImage->bits_per_pixel)
    {
    case 32:
        return QImage::Format_ARGB32;
    case 24:
        return QImage::Format_RGB888;
    default:
        LOG_MODEL_ERROR("CDisplayXLib", "Don't support format:%d", pImage->bits_per_pixel);
        return QImage::Format_Invalid;
    }
}

int CDisplayXLib::Open()
{
    int nRet = 0;
    /* Connect to a local display.
     * it defaults to the value of the DISPLAY environment variable. */
    m_pDisplay = XOpenDisplay(NULL);
    do{
        if(!m_pDisplay)
        {
            nRet = -1;
        }
       
        if (strstr(ServerVendor(m_pDisplay), "X.Org")) {
            int vendrel = VendorRelease(m_pDisplay);
            
            QString version("X.Org version: ");
            version += QString::number(vendrel / 10000000);
            version += "." + QString::number((vendrel /   100000) % 100),
                    version += "." + QString::number((vendrel /     1000) % 100);
            if (vendrel % 1000) {
                version += "." + QString::number(vendrel % 1000);
            }
            LOG_MODEL_DEBUG("CDisplayXLib", version.toStdString().c_str());
        }
        
        m_RootWindow = DefaultRootWindow(m_pDisplay); // RootWindow(dsp,0);/* Refer to the root window */
        if(0 == m_RootWindow)
        {
            LOG_MODEL_ERROR("CDisplayXLib", "cannot get root window");
            nRet = -2;
            break;
        }
    }while(0);
    
    if(nRet)
    {   
        Close();
        return nRet;
    }
    
    // Initial QImage and XImage
    Visual* vis = DefaultVisual(m_pDisplay, DefaultScreen(m_pDisplay));
    if (vis && vis->c_class == TrueColor) {
        m_pImage = XCreateImage(m_pDisplay, vis,
                            DefaultDepth(m_pDisplay, DefaultScreen(m_pDisplay)),
                            ZPixmap, 0, 0, Width(), Height(),
                            BitmapPad(m_pDisplay), 0);
        if(m_pImage)
        {
            m_pImage->data = (char *)malloc(m_pImage->bytes_per_line * m_pImage->height);
            if (m_pImage->data) 
            {
                m_Format = GetFormat(m_pImage);
                if(QImage::Format_Invalid != GetFormat())
                    m_Desktop = QImage((uchar*)m_pImage->data,
                                     Width(), Height(), GetFormat());
            }

            if(!m_pImage->data || QImage::Format_Invalid == GetFormat())
            {
                DestroyImage(m_pImage);
                m_pImage = NULL;
            }
        }
    }
    
    return nRet;
}

int CDisplayXLib::Close()
{
    int nRet = 0;
    if(m_pDisplay)
    {
        XCloseDisplay(m_pDisplay);
        m_pDisplay = NULL;
    }

    m_RootWindow = 0;
    
    if(m_pImage)
    {
        DestroyImage(m_pImage);
        m_pImage = NULL;
    }

    return nRet;
}

int CDisplayXLib::GetScreenCount()
{
    return ScreenCount(m_pDisplay);
}

void CDisplayXLib::DestroyImage(void *pImage)
{
    LOG_MODEL_DEBUG("CDisplayXLib", "void DestroyImage(void *info)");
    XDestroyImage(static_cast<XImage*>(pImage));
}

QImage CDisplayXLib::GetDisplay(int x, int y, int width, int height)
{
    if(!m_pDisplay || 0 == m_RootWindow)
        return QImage();

    if(-1 > width)
        width = Width();
    if(-1 > height)
        height = Height();
    XImage *pImage = XGetImage(m_pDisplay, m_RootWindow, x, y, width, height,
                               AllPlanes, ZPixmap);
    if(QImage::Format_Invalid == GetFormat())
        m_Format = GetFormat(pImage);
    if(QImage::Format_Invalid != GetFormat())
    {
        QImage img((uchar*)pImage->data,
                   pImage->width, pImage->height,
                   GetFormat(),
                   DestroyImage, pImage);
        return img;
    }
    else
        DestroyImage(pImage);
    
    return QImage();
}

QImage CDisplayXLib::GetDisplay()
{
    if(QImage::Format_Invalid != GetFormat()
            && m_pDisplay && m_RootWindow && m_pImage)
        XGetSubImage(m_pDisplay, m_RootWindow,
                     0, 0, Width(), Height(),
                     AllPlanes, ZPixmap,
                     m_pImage, 0, 0);
    return m_Desktop;
}
