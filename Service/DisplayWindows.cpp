#include "DisplayWindows.h"
#include <Windows.h>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logDW)
CDisplayWindows::CDisplayWindows(QObject *parent) : CDesktop(parent)
{
    m_DC = GetDesktopDC();
    m_MemDc = NULL;
    m_Bitmap = NULL;
    m_Width = m_Height = 0;
}

CDisplayWindows::~CDisplayWindows()
{
    DeleteObject(m_Bitmap);
    DeleteDC(m_MemDc);
    ReleaseDC(NULL, m_DC);
}

CDesktop* CDesktop::Instance()
{
    static CDesktop* p = nullptr;
    if(!p) p = new CDisplayWindows();
    return p;
}

HDC CDisplayWindows::GetDesktopDC()
{
    //m_DC = CreateDC(_T("DISPLAY"),NULL,NULL,NULL ); // Primary screen
    m_DC = GetDC(GetDesktopWindow()); // Multi-screen;
    return m_DC;
}

int CDisplayWindows::Width()
{
    return GetDeviceCaps(m_DC, HORZRES);  // pixel
}

int CDisplayWindows::Height()
{
    return GetDeviceCaps(m_DC, VERTRES);  // pixel
}

QImage CDisplayWindows::GetDisplay(int width, int height, int x, int y)
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
                || NULL == m_MemDc || NULL == m_Bitmap)
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
            DeleteDC(m_MemDc);
            
            m_MemDc = CreateCompatibleDC(m_DC);
            if(NULL == m_MemDc)
            {
                qCritical(logDW,
                                "CreateCompatibleDC fail: %d",
                                GetLastError());
                break;
            }
            
            m_Bitmap = CreateCompatibleBitmap(m_DC, m_Width, m_Height);
            if(NULL == m_Bitmap)
            {
                qCritical(logDW,
                                "CreateCompatibleBitmap fail: %d",
                                GetLastError());
                break;
            }
            HGDIOBJ oldBitmap = SelectObject(m_MemDc, m_Bitmap);
            if(NULL == oldBitmap)
            {
                qCritical(logDW,
                                "SelectObject fail: %d",
                                GetLastError());
                break;
            }
        }
        if(!BitBlt(m_MemDc, 0, 0, m_Width, m_Height, m_DC, x, y, SRCCOPY))
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
        if (!::GetDIBits(m_MemDc, m_Bitmap, 0, m_Height,
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
    DeleteDC(m_MemDc);
    return img;
}
