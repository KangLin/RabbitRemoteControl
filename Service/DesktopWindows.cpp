// Author: Kang Lin <kl222@126.com>

// 双屏/多显示器截屏: https://blog.csdn.net/problc/article/details/7063324
// 获取屏幕的分辨率(多屏幕或者单屏幕): https://blog.csdn.net/yousss/article/details/98848775
/*
 * Windows中接入多个显示器时，可设置为复制和扩展屏。
 * 1、设置为复制屏幕时，多个显示器的分辨率是一样的，位置为0~分辨率值
 * 2、设置为扩展屏幕时，显示器之间的关系比较复杂些。首先Windows系统会识别一个主显示器，
 * 这个可以在屏幕分辨率中更改。多个显示器之间的位置关系也可以再屏幕分辨率中更改。
 * 其中主显示器的位置为(0,0)到(width,height)，其他显示器位置由与主显示器的位置关系决定，
 * 在主显示器左上，则为负数，用0减去长宽；在右下，则由主显示器的分辨率加上长宽。
 * 其中驱动或用mouse_event处理时也是一样，主显示器为0~65535，
 * 其他显示器根据主显示器的相对位置确定。
 *
 * -----------------------------------------------------------------------------
 * |                                       Virtual Screen                     |
 * |   ------------                                                            |
 * |   |          |                                                            |
 * |   |  Screen1 |     (0,0)                                                  |
 * |   |          |    /                                                       |
 * |   ------------   /                                                        |
 * |   ------------  ------------                                              |
 * |   |          |  |          |                                              |
 * |   |  Screen2 |  |  Primary |                                              |
 * |   |          |  |  Screen3 |                                              |
 * |   ------------  ------------                                              |
 * |                                                                           |
 * |                                                                           |
 * |                                                                           |
 * |                                                                           |
 * |                                                                           |
 * |                                                                           |
 * -----------------------------------------------------------------------------
 */

#include "DesktopWindows.h"
#include <Windows.h>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(logDW, "DesktopWindows")

CDesktopWindows::CDesktopWindows(QObject *parent) : CDesktop(parent)
{
    m_DC = GetDesktopDC();
    m_MemDC = NULL;
    m_Bitmap = m_DesktopBitmap = NULL;
    m_Width = m_Height = 0;
    
    m_MemDC = CreateCompatibleDC(m_DC);
    if(m_MemDC)
    {
        m_DesktopBitmap = CreateCompatibleBitmap(m_DC, Width(), Height());
        if(NULL == m_DesktopBitmap)
            qCritical(logDW,
                            "CreateCompatibleBitmap fail: %d",
                            GetLastError());
    } else 
        qCritical(logDW,
                        "CreateCompatibleDC fail: %d",
                        GetLastError());
    
    if(m_DesktopBitmap)
    {
        HGDIOBJ oldBitmap = SelectObject(m_MemDC, m_DesktopBitmap);
        if(NULL == oldBitmap)
            qCritical(logDW, "SelectObject fail: %d", GetLastError());
        
        m_Desktop = QImage(Width(), Height(), QImage::Format_ARGB32);
    }
}

CDesktopWindows::~CDesktopWindows()
{
    if(m_Bitmap)
        DeleteObject(m_Bitmap);
    if(m_DesktopBitmap)
        DeleteObject(m_DesktopBitmap);
    if(m_MemDC)
        DeleteDC(m_MemDC);
    ReleaseDC(NULL, m_DC);
}

CDesktop* CDesktop::Instance()
{
    static CDesktop* p = nullptr;
    if(!p) p = new CDesktopWindows();
    return p;
}

HDC CDesktopWindows::GetDesktopDC()
{
    // The following methods can all get the desktop
    //m_DC = CreateDCA("DISPLAY",NULL,NULL,NULL ); // Primary screen
    //m_DC = GetWindowDC(NULL);
    m_DC = GetDC(GetDesktopWindow()); // Multi-screen;

    return m_DC;
}

int CDesktopWindows::Width()
{
    return GetSystemMetrics(SM_CXVIRTUALSCREEN);
    //return GetDeviceCaps(m_DC, HORZRES); // Primary screen pixel
}

int CDesktopWindows::Height()
{
    return GetSystemMetrics(SM_CYVIRTUALSCREEN);
    //return GetDeviceCaps(m_DC, VERTRES); // Primary screen pixel
}

QImage CDesktopWindows::GetDesktop()
{
    do{
        if(!BitBlt(m_MemDC, 0, 0, Width(), Height(), m_DC, 0, 0, SRCCOPY))
        {
            qCritical(logDW,
                            "BitBlt fail: %d",
                            GetLastError());
            break;
        }
        
        //SelectObject(memDc, oldBitmap);
        
        // Get the bitmap format information
        BITMAPINFO bi;
        memset(&bi, 0, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biBitCount = 0;
        
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = Width();
        bi.bmiHeader.biHeight = Height();
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        bi.bmiHeader.biSizeImage = 0;
        bi.bmiHeader.biXPelsPerMeter = 0;
        bi.bmiHeader.biYPelsPerMeter = 0;
        bi.bmiHeader.biClrUsed = 0;
        bi.bmiHeader.biClrImportant = 0;
        
        // Get image
        if (!::GetDIBits(m_MemDC, m_DesktopBitmap, 0, Height(),
                         m_Desktop.bits(), (BITMAPINFO*)&bi, DIB_RGB_COLORS))
        {
            qCritical(logDW,
                            "Get image fail: %d",
                            GetLastError());
            break;
        }
        
        m_Desktop = m_Desktop.mirrored();
    } while(0);
    
    return m_Desktop;
}

QImage CDesktopWindows::GetDesktop(int x, int y, int width, int height)
{
    QImage img;
    bool bChange = false;
    
    do{
        if(NULL == m_DC)
        {
            qCritical(logDW,
                            "GetDC fail: %d",
                            GetLastError());
            return img;
        }
        
        if(width != m_Width || height != m_Height
                || NULL == m_MemDC || NULL == m_Bitmap)
        {
            bChange = true;
            m_Width = width;
            m_Height = height;
            if(-1 == width) m_Width = Width();
            if(-1 == height) m_Height = Height();
        }
        
        if(bChange)
        {
            DeleteObject(m_Bitmap);

            m_Bitmap = CreateCompatibleBitmap(m_DC, m_Width, m_Height);
            if(NULL == m_Bitmap)
            {
                qCritical(logDW,
                                "CreateCompatibleBitmap fail: %d",
                                GetLastError());
                break;
            }
            HGDIOBJ oldBitmap = SelectObject(m_MemDC, m_Bitmap);
            if(NULL == oldBitmap)
            {
                qCritical(logDW,
                                "SelectObject fail: %d",
                                GetLastError());
                break;
            }
        }
        
        if(!BitBlt(m_MemDC, 0, 0, m_Width, m_Height, m_DC, x, y, SRCCOPY))
        {
            qCritical(logDW,
                            "BitBlt fail: %d",
                            GetLastError());
            break;
        }
        
        //SelectObject(memDc, oldBitmap);
        
        // Get the bitmap format information
        BITMAPINFO bi;
        memset(&bi, 0, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biBitCount = 0;
        
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = m_Width;
        bi.bmiHeader.biHeight = m_Height;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        bi.bmiHeader.biSizeImage = 0;
        bi.bmiHeader.biXPelsPerMeter = 0;
        bi.bmiHeader.biYPelsPerMeter = 0;
        bi.bmiHeader.biClrUsed = 0;
        bi.bmiHeader.biClrImportant = 0;
        
        img = QImage(m_Width, m_Height, QImage::Format_ARGB32);
        
        // Get image
        if (!::GetDIBits(m_MemDC, m_Bitmap, 0, m_Height,
                         img.bits(), (BITMAPINFO*)&bi, DIB_RGB_COLORS))
        {
            qCritical(logDW,
                            "Get image fail: %d",
                            GetLastError());
            break;
        }
        
        img = img.mirrored();
    } while(0);
    
    DeleteObject(m_Bitmap);

    return img;
}

QImage CDesktopWindows::GetCursor(QPoint &pos, QPoint &posHot)
{
    QImage img;
    return img;
}

QPoint CDesktopWindows::GetCursorPosition()
{
    QPoint pos;
    return pos;
}
