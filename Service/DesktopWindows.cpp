#include "DesktopWindows.h"
#include <Windows.h>
#include "RabbitCommonLog.h"

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
            LOG_MODEL_ERROR("Screen",
                            "CreateCompatibleBitmap fail: %d",
                            GetLastError());
    } else 
        LOG_MODEL_ERROR("Screen",
                        "CreateCompatibleDC fail: %d",
                        GetLastError());
    
    if(m_DesktopBitmap)
    {
        HGDIOBJ oldBitmap = SelectObject(m_MemDC, m_DesktopBitmap);
        if(NULL == oldBitmap)
            LOG_MODEL_ERROR("Screen", "SelectObject fail: %d", GetLastError());
        
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
    //m_DC = CreateDC(_T("DISPLAY"),NULL,NULL,NULL ); // Primary screen
    m_DC = GetDC(GetDesktopWindow()); // Multi-screen;
    return m_DC;
}

int CDesktopWindows::Width()
{
    return GetDeviceCaps(m_DC, HORZRES);  // pixel
}

int CDesktopWindows::Height()
{
    return GetDeviceCaps(m_DC, VERTRES);  // pixel
}

QImage CDesktopWindows::GetDesktop()
{
    do{
        if(!BitBlt(m_MemDC, 0, 0, Width(), Height(), m_DC, 0, 0, SRCCOPY))
        {
            LOG_MODEL_ERROR("Screen",
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
            LOG_MODEL_ERROR("Screen",
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
            LOG_MODEL_ERROR("Screen",
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
                LOG_MODEL_ERROR("Screen",
                                "CreateCompatibleBitmap fail: %d",
                                GetLastError());
                break;
            }
            HGDIOBJ oldBitmap = SelectObject(m_MemDC, m_Bitmap);
            if(NULL == oldBitmap)
            {
                LOG_MODEL_ERROR("Screen",
                                "SelectObject fail: %d",
                                GetLastError());
                break;
            }
        }
        
        if(!BitBlt(m_MemDC, 0, 0, m_Width, m_Height, m_DC, x, y, SRCCOPY))
        {
            LOG_MODEL_ERROR("Screen",
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
            LOG_MODEL_ERROR("Screen",
                            "Get image fail: %d",
                            GetLastError());
            break;
        }
        
        img = img.mirrored();
    } while(0);
    
    DeleteObject(m_Bitmap);

    return img;
}
