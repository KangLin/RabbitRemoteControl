#include "DesktopXLib.h"
#include "ScreenXLib.h"
#include "Display.h"

CDesktopXLib::CDesktopXLib(QObject *parent) : CDesktop(parent)
{
}

CDesktop* CDesktop::Instance()
{
    static CDesktop* p = nullptr;
    if(!p) p = new CDesktopXLib();
    return p;
}

int CDesktopXLib::Width()
{
    return CDisplay::Instance()->Width();
}

int CDesktopXLib::Height()
{
    return CDisplay::Instance()->Height();
}

QImage CDesktopXLib::GetDesktop()
{
    return CDisplay::Instance()->GetDisplay();
}

QImage CDesktopXLib::GetDesktop(int x, int y, int width, int height)
{
    return CDisplay::Instance()->GetDisplay();
}

QImage CDesktopXLib::GetCursor()
{
    return CDisplay::Instance()->GetCursor();
}

QPoint CDesktopXLib::GetCursorPosition()
{
    return CDisplay::Instance()->GetCursorPosition();
}
