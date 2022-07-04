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
    return CDisplay::Instance()->GetDisplay(x, y, width, height);
}

QImage CDesktopXLib::GetCursor(QPoint &pos, QPoint &posHot)
{
    return CDisplay::Instance()->GetCursor(pos, posHot);
}

QPoint CDesktopXLib::GetCursorPosition()
{
    return CDisplay::Instance()->GetCursorPosition();
}

bool CDesktopXLib::GetHasCursor() const
{
    return CDisplay::Instance()->GetHasCursor();
}

void CDesktopXLib::SetHasCursor(bool hasCursor)
{
    return CDisplay::Instance()->SetHasCursor(hasCursor);
}
