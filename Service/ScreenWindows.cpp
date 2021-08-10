#include "Screen.h"
#include <Windows.h>

CScreen::CScreen(QObject *parent) : QObject(parent)
{
}

int CScreen::Width()
{
    return GetSystemMetrics(SM_CXSCREEN);
}

int CScreen::Height()
{
    return GetSystemMetrics(SM_CYSCREEN);
}

int CScreen::VirtualTop()
{
    return GetSystemMetrics(SM_XVIRTUALSCREEN);
}

int CScreen::VirtualLeft()
{
    return GetSystemMetrics(SM_YVIRTUALSCREEN);
}

int CScreen::VirtualWidth()
{
    return GetSystemMetrics(SM_CXVIRTUALSCREEN);
}

int CScreen::VirtualHeight()
{
    return GetSystemMetrics(SM_CYVIRTUALSCREEN);
}
