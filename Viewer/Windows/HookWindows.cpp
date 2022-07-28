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

// See: https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644985(v=vs.85)
LRESULT CALLBACK CHookWindows::keyboardHookProc(INT code, WPARAM wparam, LPARAM lparam)
{
    if (code == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* hook = reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);
//        LOG_MODEL_DEBUG("CHookWindows", "vkCode: 0x%X; scanCode: 0x%X; flags: 0x%X",
//                        hook->vkCode, hook->scanCode, hook->flags);
        int key = 0;
        Qt::KeyboardModifiers keyMdf = Qt::NoModifier;
        switch(hook->vkCode)
        {
        case VK_TAB:
        {
            key = Qt::Key_Tab;
            break;
        }
//        case VK_DELETE:
//        {
//            key = Qt::Key_Delete;
//            break;
//        }
        case VK_LWIN:
        {
            key = Qt::Key_Super_L;
            break;
        }
        case VK_RWIN:
        {
            key = Qt::Key_Super_R;
            break;
        }
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
        {
            key = Qt::Key_Menu;
            keyMdf = Qt::KeyboardModifier::AltModifier;
            break;
        }
            /*
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        {
            key = Qt::Key_Control;
            keyMdf = Qt::KeyboardModifier::ControlModifier;
            break;
        }
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        {
            key = Qt::Key_Shift;
            keyMdf = Qt::KeyboardModifier::ShiftModifier;
            break;
        }*/
        default:
            break;
        }
        if(key) {
            CFrmViewer* self = dynamic_cast<CFrmViewer*>(QApplication::focusWidget());
            WId foreground_window = reinterpret_cast<WId>(GetForegroundWindow());
            if (self && self->parentWidget()->window()->winId() == foreground_window)
            {
                if(wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
                    emit self->sigKeyPressEvent(key, keyMdf);
                if(wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
                    emit self->sigKeyReleaseEvent(key, Qt::NoModifier);
//                LOG_MODEL_DEBUG("CHookWindows", "process vkCode: 0x%X; scanCode: 0x%X; flags: 0x%X",
//                                hook->vkCode, hook->scanCode, hook->flags);
                return true;
            }
        }
    }

    return CallNextHookEx(nullptr, code, wparam, lparam);
}

// See: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw
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
