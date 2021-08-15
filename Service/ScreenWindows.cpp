#include "ScreenWindows.h"
#include <Windows.h>
#include "RabbitCommonLog.h"

CScreen* CScreen::Instance()
{
    static CScreen* p = nullptr;
    if(!p) p = new CScreenWindows();
    return p;
}

CScreenWindows::CScreenWindows(QObject *parent) : CScreen(parent)
{
}

int CScreenWindows::Width()
{
    if(m_Screen.isNull())
        GetImage(false);
    if(m_Screen.isNull())
        return GetSystemMetrics(SM_CXSCREEN);
    return m_Screen.width();
}

int CScreenWindows::Height()
{
    if(m_Screen.isNull())
        GetImage(false);
    if(m_Screen.isNull())
        return GetSystemMetrics(SM_CYSCREEN);
    return m_Screen.height();
}

int CScreenWindows::VirtualTop()
{
    return GetSystemMetrics(SM_XVIRTUALSCREEN);
}

int CScreenWindows::VirtualLeft()
{
    return GetSystemMetrics(SM_YVIRTUALSCREEN);
}

int CScreenWindows::VirtualWidth()
{
    return GetSystemMetrics(SM_CXVIRTUALSCREEN);
}

int CScreenWindows::VirtualHeight()
{
    return GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

int CScreenWindows::VisibleMonitorCount()
{
    return GetSystemMetrics(SM_CMONITORS);
}

QImage::Format CScreenWindows::GetFormat(int index)
{
    if(QImage::Format_Invalid == m_Format)
    {
        GetImage(false);
        m_Format = m_Screen.format();
    }
    return m_Format;
}

QImage CScreenWindows::GetScreen(int index)
{
    int nRet = GetImage(true);
    if(nRet) return QImage();
    
    if(QImage::Format_Invalid == m_Format)
        m_Format = m_Screen.format();
    if(m_Screen.format() != m_Format && QImage::Format_Invalid != m_Format)
        m_Screen = m_Screen.convertToFormat(m_Format);
    return m_Screen;
}

int CScreenWindows::SetFormat(QImage::Format f)
{    
    m_Format = f;
    return 0;
}

int CScreenWindows::GetImage(bool bBuffer)
{
    int nRet = -1;
    
    if(!m_Screen.isNull() && !bBuffer)
        return 0;
    
    HWND hwnd = NULL;
    HDC dc = NULL;
    HDC memDc = NULL;
    HBITMAP bitmap = NULL;
    
    do{
        //hwnd = GetDesktopWindow();
        dc = GetDC(hwnd);
        if(NULL == dc)
        {
            LOG_MODEL_ERROR("Screen",
                            "GetDC fail: %d",
                            GetLastError());
            break;
        }
        
        memDc = CreateCompatibleDC(dc);
        if(NULL == memDc)
        {
            LOG_MODEL_ERROR("Screen",
                            "CreateCompatibleDC fail: %d",
                            GetLastError());
            break;
        }
        
        int ScreenWidth = GetDeviceCaps(dc, HORZRES);
        int ScreenHeight = GetDeviceCaps(dc, VERTRES);
        bitmap = CreateCompatibleBitmap(dc, ScreenWidth, ScreenHeight);
        if(NULL == bitmap)
        {
            LOG_MODEL_ERROR("Screen",
                            "CreateCompatibleBitmap fail: %d",
                            GetLastError());
            break;
        }
        HGDIOBJ oldBitmap = SelectObject(memDc, bitmap);
        if(NULL == oldBitmap)
        {
            LOG_MODEL_ERROR("Screen",
                            "SelectObject fail: %d",
                            GetLastError());
            break;
        }
        if(!BitBlt(memDc, 0, 0, ScreenWidth, ScreenHeight, dc, 0, 0, SRCCOPY))
        {
            LOG_MODEL_ERROR("Screen",
                            "BitBlt fail: %d",
                            GetLastError());
            break;
        }
        
        //        //添加鼠标到图片里
        //        POINT l_pt;
        //        ::GetCursorPos(&l_pt);
        //        HCURSOR l_hCursor = ::GetCursor();
        //        ::DrawIcon(memDc, l_pt.x, l_pt.y, l_hCursor); 
        
        //SelectObject(memDc, oldBitmap);
        
        // Get the bitmap format information
        BITMAPINFO bi;
        memset(&bi, 0, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biBitCount = 0;
        
        // Get image format
        if (!::GetDIBits(memDc, bitmap, 0, ScreenHeight, NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS))
        {
            LOG_MODEL_ERROR("Screen",
                            "unable to determine device pixel format: %d",
                            GetLastError());
            break;
        }
        //        if (!::GetDIBits(memDc, bitmap, 0, ScreenHeight, NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS))
        //        {
        //            LOG_MODEL_ERROR("Screen",
        //                            "unable to determine pixel shifts/palette: %d",
        //                            GetLastError());
        //            break;
        //        }
        
        if(bi.bmiHeader.biBitCount > 8)
        {
            switch(bi.bmiHeader.biCompression)
            {
            case BI_RGB:
                // Default RGB layout
                switch (bi.bmiHeader.biBitCount) {
                case 16:
                    // RGB 555 - High Colour
                    LOG_MODEL_INFO("Screen", "16-bit High Colour");
                    m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_RGB555);
                    break;
                case 24:
                    LOG_MODEL_INFO("Screen", "24-bit High Colour");
                    m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_RGB888);
                    break; 
                case 32:
                    // RGB 888 - True Colour
                    LOG_MODEL_INFO("Screen", "32-bit High Colour");
                    m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_ARGB32);
                    break; 
                default:
                    LOG_MODEL_ERROR("Screen","bits per pixel %u not supported", bi.bmiHeader.biBitCount);
                    break;
                };
                break;
            case BI_BITFIELDS:
                // Default RGB layout
                switch (bi.bmiHeader.biBitCount) {
                case 16:
                    // RGB 555 - High Colour
                    LOG_MODEL_INFO("Screen", "16-bit High Colour");
                    m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_RGB555);
                    break;
                case 32:
                    // RGB 888 - True Colour
                    LOG_MODEL_INFO("Screen", "32-bit High Colour");
                    m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_ARGB32);
                    break; 
                default:
                    LOG_MODEL_ERROR("Screen","bits per pixel %u not supported", bi.bmiHeader.biBitCount);
                    break;
                };
                break;
            }
        } else {
            LOG_MODEL_ERROR("Screen", "bi.bmiHeader.biBitCount < 8");
        }
        
        if(bBuffer)
        {
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biWidth = ScreenWidth;
            bi.bmiHeader.biHeight = ScreenHeight;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biCompression = BI_RGB;
            bi.bmiHeader.biSizeImage = 0;
            bi.bmiHeader.biXPelsPerMeter = 0;
            bi.bmiHeader.biYPelsPerMeter = 0;
            bi.bmiHeader.biClrUsed = 0;
            bi.bmiHeader.biClrImportant = 0;
            // Get image
            if (!::GetDIBits(memDc, bitmap, 0, ScreenHeight,
                             m_Screen.bits(), (BITMAPINFO*)&bi, DIB_RGB_COLORS))
            {
                LOG_MODEL_ERROR("Screen",
                                "Get image fail: %d",
                                GetLastError());
                break;
            }
            
            m_Screen = m_Screen.mirrored();
        }
        
        nRet = 0;
    } while(0);
    
    DeleteObject(bitmap);
    DeleteDC(memDc);
    ReleaseDC(NULL, dc);
    
    return nRet;
}
