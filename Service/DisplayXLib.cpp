// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "DisplayXLib.h"

#ifdef HAVE_Xfixes
#include <X11/extensions/Xfixes.h>
#endif

// xlib documents: https://www.x.org/releases/current/doc/libX11/libX11/libX11.html
// X documents: https://www.x.org/releases/current/doc/index.html
// X Window System Concepts: https://www.x.org/wiki/guide/concepts/
// https://www.x.org

#include <QPainter>
#include <QBitmap>

Q_DECLARE_LOGGING_CATEGORY(LogScreen)

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
        qCritical(LogScreen) << "Don't support format:" << pImage->bits_per_pixel;
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
            qInfo(LogScreen) << version;
        }
        
        m_RootWindow = DefaultRootWindow(m_pDisplay); // RootWindow(dsp,0);/* Refer to the root window */
        if(0 == m_RootWindow)
        {
            qCritical(LogScreen) << "cannot get root window";
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
        if(m_pImage) {
            m_pImage->data = (char *)malloc(m_pImage->bytes_per_line * m_pImage->height);
            if (m_pImage->data) {
                m_Format = GetFormat(m_pImage);
                if(QImage::Format_Invalid != GetFormat()) {
                    m_Desktop = QImage((uchar*)m_pImage->data,
                                     Width(), Height(), GetFormat());
                    //m_Desktop.fill(QColor(0,0,0,0));
                }
            }

            if(!m_pImage->data || QImage::Format_Invalid == GetFormat()) {
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
    qDebug(LogScreen) << "void DestroyImage(void *info)";
    XDestroyImage(static_cast<XImage*>(pImage));
}

QImage CDisplayXLib::GetDisplay(int x, int y, int width, int height)
{
    if(!m_pDisplay || 0 == m_RootWindow)
        return QImage();

    if(0 > width)
        width = Width();
    if(0 > height)
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
        
        if(GetHasCursor())
        {
            QPoint pos, posHot;
            QImage cursor = GetCursor(pos, posHot);
            if(!cursor.isNull()
                    && pos.x() >= x && (pos.x() + cursor.width()) <= width
                    && pos.y() >= y && (pos.y() + cursor.height()) <= height)
            {
                QPainter painter(&img);
                painter.drawImage(pos, cursor);
            }
        }
        
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
    {
        XImage* pImg = XGetSubImage(m_pDisplay, m_RootWindow,
                     0, 0, Width(), Height(),
                     AllPlanes, ZPixmap,
                     m_pImage, 0, 0);
        if(NULL == pImg)
        {
            qCritical(LogScreen) << "Get desktop fail";
        }
    }

    if(GetHasCursor())
    {
        QPoint pos, posHot;
        QImage cursor = GetCursor(pos, posHot);
        if(!cursor.isNull())
        {
            if(m_Desktop.format() != cursor.format())
                cursor = cursor.convertToFormat(m_Desktop.format());
            QPainter painter(&m_Desktop);
            painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
            painter.drawImage(pos, cursor);
        }
    }
    return m_Desktop;
}

QImage CDisplayXLib::GetCursor(QPoint &pos, QPoint &posHot)
{
#ifdef HAVE_Xfixes
    
    XFixesCursorImage* ci;
    XLockDisplay(m_pDisplay);
    ci = XFixesGetCursorImage(m_pDisplay);
    XUnlockDisplay(m_pDisplay);
    if (!ci)
        return QImage();
    
    pos = QPoint(ci->x, ci->y);
    posHot = QPoint(ci->xhot, ci->yhot);
    QImage img(ci->width, ci->height, QImage::Format_ARGB32);

    unsigned char r = 0, g = 0, b = 0, a = 0;
    unsigned short row = 0, col = 0, k = 0;

    for(row = 0; row < ci->height; row++)
    {
        for(col = 0; col < ci->width; col++, k++)
        {
            a = (unsigned char)((ci->pixels[k] >> 24) & 0xff);
            r = (unsigned char)((ci->pixels[k] >> 16) & 0xff);
            g = (unsigned char)((ci->pixels[k] >>  8) & 0xff);
            b = (unsigned char)((ci->pixels[k] >>  0) & 0xff);

            QColor c(r, g, b, a);
            img.setPixelColor(col, row, c);
        }
    }

    XFree(ci);

    return img;
#else
    pos = GetCursorPosition();
#endif
    return QImage();
}

QPoint CDisplayXLib::GetCursorPosition()
{
    unsigned int mask;
    int win_x = 0, win_y = 0;
    int root_x = 0, root_y = 0;
    Window root, child;
    XLockDisplay(m_pDisplay);

    if (!XQueryPointer(m_pDisplay, m_RootWindow, &root, &child, &root_x,
                       &root_y, &win_x, &win_y, &mask))
    {
        XUnlockDisplay(m_pDisplay);
        return QPoint();
    }

    XUnlockDisplay(m_pDisplay);
    return QPoint(root_x, root_y);
}
