#include "HookWindows.h"
#include "RabbitCommonLog.h"
#include "FrmViewer.h"
#include <QApplication>
#include <QDebug>

CHookWindows::CHookWindows(QObject *parent)
    : CHook(parent),
      m_hKeyboard(nullptr)
{
    RegisterKeyboard();
}

CHookWindows::~CHookWindows()
{
    qDebug() << "CHookWindows::~CHookWindows()";
    UnRegisterKeyboard();
}

LRESULT CALLBACK CHookWindows::keyboardHookProc(INT code, WPARAM wparam, LPARAM lparam)
{
    if (code == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* hook = reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);
        LOG_MODEL_DEBUG("CFrmViewer", "vkCode: 0x%X; scanCode: 0x%X; flags: 0x%X",
                        hook->vkCode, hook->scanCode, hook->flags);
        switch(hook->vkCode)
        {
        case VK_LWIN:
        case VK_RWIN:
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        {
            CFrmViewer* self = dynamic_cast<CFrmViewer*>(QApplication::focusWidget());
            WId foreground_window = reinterpret_cast<WId>(GetForegroundWindow());
            if (self && self->parentWidget()->window()->winId() == foreground_window)
            {
                LOG_MODEL_DEBUG("CFrmViewer", "process vkCode: 0x%X; scanCode: 0x%X; flags: 0x%X",
                                hook->vkCode, hook->scanCode, hook->flags);
                return true;
            }
        }
        default:
            break;
        }
    }

    return CallNextHookEx(nullptr, code, wparam, lparam);
}

int CHookWindows::RegisterKeyboard()
{
    if(m_hKeyboard)
        UnRegisterKeyboard();
    m_hKeyboard = SetWindowsHookExW(WH_KEYBOARD_LL, keyboardHookProc, nullptr, 0);
    return 0;
}

int CHookWindows::UnRegisterKeyboard()
{
    if(m_hKeyboard)
    {
        UnhookWindowsHookEx(m_hKeyboard);
        m_hKeyboard = nullptr;
    }
    return 0;
}
