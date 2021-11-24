#include "DesktopXLib.h"
#include "ScreenXLib.h"

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
    return CScreen::Instance()->Width();
}

int CDesktopXLib::Height()
{
    return CScreen::Instance()->Height();
}

QImage CDesktopXLib::GetDesktop(int width, int height, int x, int y)
{
    return CScreen::Instance()->GetScreen();
}
