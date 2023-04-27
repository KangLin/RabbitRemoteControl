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

#include "ScreenWindows.h"
#include <Windows.h>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(logDW)

CScreen* CScreen::Instance()
{
    static CScreen* p = nullptr;
    if(!p) p = new CScreenWindows();
    return p;
}

CScreenWindows::CScreenWindows(QObject *parent) : CScreen(parent)
{
    m_Format = QImage::Format_ARGB32;
}

CScreenWindows::~CScreenWindows()
{
    qDebug(logDW, "CScreenWindows::~CScreenWindows");
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
    return m_Format;
}

int CScreenWindows::SetFormat(QImage::Format f)
{
    m_Format = f;
    return 0;
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

int CScreenWindows::GetImage(bool bBuffer)
{
    int nRet = -1;
    
    if(!m_Screen.isNull() && !bBuffer)
        return 0;

    HDC dc = NULL;
    HDC memDc = NULL;
    HBITMAP bitmap = NULL;

    do{
        //dc = = CreateDC( _T("DISPLAY"),NULL,NULL,NULL ); // Primary screen
        dc = GetDC(GetDesktopWindow()); // Multi-screen
        if(NULL == dc)
        {
            qCritical(logDW,
                            "GetDC fail: %d",
                            GetLastError());
            break;
        }
        
        memDc = CreateCompatibleDC(dc);
        if(NULL == memDc)
        {
            qCritical(logDW,
                            "CreateCompatibleDC fail: %d",
                            GetLastError());
            break;
        }
        
        int ScreenWidth = GetDeviceCaps(dc, HORZRES);  // pixel
        int ScreenHeight = GetDeviceCaps(dc, VERTRES); // pixel
        bitmap = CreateCompatibleBitmap(dc, ScreenWidth, ScreenHeight);
        if(NULL == bitmap)
        {
            qCritical(logDW,
                            "CreateCompatibleBitmap fail: %d",
                            GetLastError());
            break;
        }
        HGDIOBJ oldBitmap = SelectObject(memDc, bitmap);
        if(NULL == oldBitmap)
        {
            qCritical(logDW,
                            "SelectObject fail: %d",
                            GetLastError());
            break;
        }
        if(!BitBlt(memDc, 0, 0, ScreenWidth, ScreenHeight, dc, 0, 0, SRCCOPY))
        {
            qCritical(logDW,
                            "BitBlt fail: %d",
                            GetLastError());
            break;
        }
        
        if(HasCursor())
        {
            //TODO: There is a bug
            POINT point;
            ::GetCursorPos(&point);
            HCURSOR hCursor = ::GetCursor();
            ::DrawIcon(memDc, point.x, point.y, hCursor); 
        }
        
        //SelectObject(memDc, oldBitmap);
        
        // Get the bitmap format information
        BITMAPINFO bi;
        memset(&bi, 0, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biBitCount = 0;
        
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

            if(m_Screen.isNull() || m_Screen.format() != QImage::Format_ARGB32)
                m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_ARGB32);

            // Get image
            if (!::GetDIBits(memDc, bitmap, 0, ScreenHeight,
                             m_Screen.bits(), (BITMAPINFO*)&bi, DIB_RGB_COLORS))
            {
                qCritical(logDW,
                                "Get image fail: %d",
                                GetLastError());
                break;
            }
            
            m_Screen = m_Screen.mirrored();
            nRet = 0;
        } else {
            // Get image format
            if (!::GetDIBits(memDc, bitmap, 0, ScreenHeight, NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS))
            {
                qCritical(logDW,
                                "unable to determine device pixel format: %d",
                                GetLastError());
                break;
            }
            //        if (!::GetDIBits(memDc, bitmap, 0, ScreenHeight, NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS))
            //        {
            //            qCritical(logDW,
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
                        qInfo(logDW, "16-bit High Colour");
                        m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_RGB555);
                        break;
                    case 24:
                        qInfo(logDW, "24-bit High Colour");
                        m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_RGB888);
                        break; 
                    case 32:
                        // RGB 888 - True Colour
                        qInfo(logDW, "32-bit High Colour");
                        m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_ARGB32);
                        break; 
                    default:
                        qCritical(logDW, "bits per pixel %u not supported", bi.bmiHeader.biBitCount);
                        break;
                    };
                    break;
                case BI_BITFIELDS:
                    // Default RGB layout
                    switch (bi.bmiHeader.biBitCount) {
                    case 16:
                        // RGB 555 - High Colour
                        qInfo(logDW, "16-bit High Colour");
                        m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_RGB555);
                        break;
                    case 32:
                        // RGB 888 - True Colour
                        qInfo(logDW, "32-bit High Colour");
                        m_Screen = QImage(ScreenWidth, ScreenHeight, QImage::Format_ARGB32);
                        break; 
                    default:
                        qCritical(logDW, "bits per pixel %u not supported", bi.bmiHeader.biBitCount);
                        break;
                    };
                    break;
                }
                
                nRet = 0;
            } else {
                qCritical(logDW, "bi.bmiHeader.biBitCount < 8");
            }
        }
        
    } while(0);
    
    DeleteObject(bitmap);
    DeleteDC(memDc);
    ReleaseDC(NULL, dc);
    
    return nRet;
}
