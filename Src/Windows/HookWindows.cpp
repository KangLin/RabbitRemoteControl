// Author: Kang Lin <kl222@126.com>

#include <QApplication>
#include <QKeyEvent>
#include <QLoggingCategory>
#include <RabbitCommonTools.h>
#include <QMessageBox>
#include <QCheckBox>

#include "FrmViewer.h"
#include "HookWindows.h"
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "Plugin.Hook.Windows")

CHookWindows::CHookWindows(CParameterPlugin *pParaClient, QObject *parent)
    : CHook(pParaClient, parent),
    m_hKeyboard(nullptr)
{
}

CHookWindows::~CHookWindows()
{
    qDebug(log) << "CHookWindows::~CHookWindows()";
}

// See: https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/ms644985(v=vs.85)
LRESULT CALLBACK CHookWindows::keyboardHookProc(INT code, WPARAM wparam, LPARAM lparam)
{
    if (code == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* hook = reinterpret_cast<KBDLLHOOKSTRUCT*>(lparam);
        /*
        qDebug(log) << "process vkCode:" << hook->vkCode
                                  << "scanCode:" << hook->scanCode
                                  << "flags:" << hook->flags;//*/
        int key = 0;
        Qt::KeyboardModifiers keyMdf = Qt::NoModifier;
        // See: [virtual-key-codes](https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes)
        switch(hook->vkCode)
        {
        case VK_TAB:
        {
            key = Qt::Key_Tab;
            break;
        }
        case VK_DELETE:
        {
            key = Qt::Key_Delete;
            break;
        }
        case VK_LWIN:
        {
            key = Qt::Key_Super_L;
            keyMdf = Qt::MetaModifier;
            break;
        }
        case VK_RWIN:
        {
            key = Qt::Key_Super_R;
            keyMdf = Qt::MetaModifier;
            break;
        }
        case VK_MENU:
        case VK_LMENU:
        case VK_RMENU:
        {
            key = Qt::Key_Alt;
            //keyMdf = Qt::KeyboardModifier::AltModifier;
            break;
        }
        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
        {
            key = Qt::Key_Control;
            //keyMdf = Qt::KeyboardModifier::ControlModifier;
            break;
        }
        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
        {
            key = Qt::Key_Shift;
            //keyMdf = Qt::KeyboardModifier::ShiftModifier;
            break;
        }
        default:
            break;
        }
        if(key) {
            CFrmViewer* self = qobject_cast<CFrmViewer*>(QApplication::focusWidget());
            WId foreground_window = reinterpret_cast<WId>(GetForegroundWindow());
            if (self && self->parentWidget()->window()->winId() == foreground_window)
            {
                if(wparam == WM_KEYDOWN || wparam == WM_SYSKEYDOWN)
                    emit self->sigKeyPressEvent(new QKeyEvent(QKeyEvent::KeyPress, key, keyMdf));
                    //QApplication::postEvent(self, new QKeyEvent(QKeyEvent::KeyPress, key, keyMdf));
                if(wparam == WM_KEYUP || wparam == WM_SYSKEYUP)
                    emit self->sigKeyReleaseEvent(new QKeyEvent(QKeyEvent::KeyRelease, key, Qt::NoModifier));
                    //QApplication::postEvent(self, new QKeyEvent(QKeyEvent::KeyRelease, key, Qt::NoModifier));
                //*
                qDebug(log) << "process vkCode:" << hook->vkCode
                                          << "scanCode:" << hook->scanCode
                                          << "flags:" << hook->flags;//*/
                /* the hook procedure did not process the message,
                 * it is highly recommended that you call CallNextHookEx
                 * and return the value it returns; otherwise,
                 * other applications that have installed WH_KEYBOARD_LL hooks
                 * will not receive hook notifications and may behave incorrectly
                 * as a result. If the hook procedure processed the message,
                 * it may return a nonzero value to prevent the system 
                 * from passing the message to the rest of the hook chain
                 * or the target window procedure.
                 */
                return 1;
            }
        }
    }

    return CallNextHookEx(nullptr, code, wparam, lparam);
}

// See: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexw
int CHookWindows::OnRegisterKeyboard()
{
    if(m_hKeyboard)
        UnRegisterKeyboard();
    m_hKeyboard = SetWindowsHookExW(WH_KEYBOARD_LL, keyboardHookProc, nullptr, 0);
    if(NULL == m_hKeyboard) {
        qCritical(log) << "SetWindowsHookExW error:" << GetLastError();
        return -1;
    }
    return 0;
}

int CHookWindows::OnUnRegisterKeyboard()
{
    if(m_hKeyboard)
    {
        UnhookWindowsHookEx(m_hKeyboard);
        m_hKeyboard = nullptr;
    }
    return 0;
}

int CHookWindows::OnDisableDesktopShortcuts()
{
    DisableTaskManager(true);
    return 0;
}

int CHookWindows::OnRestoreDesktopShortcuts()
{
    DisableTaskManager(false);
    return 0;
}

// 启用备份和恢复权限
bool EnableDebugPrivileges() {
    HANDLE hToken = NULL;
    LUID sedebugnameValue;
    TOKEN_PRIVILEGES tkp;

    // 获取当前进程的访问令牌
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        qDebug(log) << "OpenProcessToken failed. Error:" << GetLastError();
        return false;
    }

    do {
        // 查找 SE_DEBUG_NAME 权限的LUID
        if (!LookupPrivilegeValue(NULL, SE_RESTORE_NAME, &sedebugnameValue)) {
            qDebug(log) << "LookupPrivilegeValue (SE_DEBUG_NAME) failed. Error:" << GetLastError();
            break;
        }

        // 设置权限结构
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Luid = sedebugnameValue;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        // 调整权限
        if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) {
            qDebug(log) << "AdjustTokenPrivileges failed. Error:" << GetLastError();
            break;
        }

        // 检查权限是否成功启用
        if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
            qDebug(log) << "The token does not have the specified privileges.";
            break;
        }
        qDebug(log) << "Debug privileges enabled successfully.";
    }while(0);

    CloseHandle(hToken);
    return true;
}

//添加注册屏蔽 Ctrl+Alt+del
//在注册表该目录下增加新内容
#define TASKMANAGERSystem "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"
#define TASKMANAGERExplorer "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"
#define WINLOGON "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon"
void CHookWindows::DisableTaskManager(bool flag)
{
    // 屏蔽ctrl + alt +del 需要修改注册表的值， 取得管理员权限， 关闭360等杀毒软件
    int value = flag ? 0x00000001 : 0x00000000;
    QSettings system(TASKMANAGERSystem, QSettings::NativeFormat);
    system.setValue("DisableTaskMgr", value); //任务管理器
    system.setValue("DisableChangePassword", value); //更改密码
    system.setValue("DisableSwitchUserOption", value); //切换用户
    system.setValue("DisableCAD", value); // Disable ctrl+alt+del
    system.setValue("DisableLockWorkstation", value); //锁定计算机

    QSettings explorer(TASKMANAGERExplorer, QSettings::NativeFormat);
    explorer.setValue("NoLogOff", value); //注销

    /*
    QSettings winlogon(WINLOGON, QSettings::NativeFormat);
    winlogon.setValue("DisableCAD", value); // Disable ctrl+alt+del
    winlogon.setValue("DisableLockWorkstation", value); //锁定计算机
    //*/
}

bool CHookWindows::DisableWindowsKey()
{
    // 方法1: 使用 RegisterHotKey 来占用 Windows 键
    bool success = true;

    // 注册热键来拦截 Windows 键
    if (!RegisterHotKey(nullptr, 1, MOD_WIN, VK_LWIN)) {
        qCritical(log) << "注册左 Windows 键拦截失败:" << GetLastError();
        success = false;
    }

    if (!RegisterHotKey(nullptr, 2, MOD_WIN, VK_RWIN)) {
        qCritical(log) << "注册右 Windows 键拦截失败:" << GetLastError();
        success = false;
    }

    // 方法2: 修改注册表禁用 Windows 键
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER,
                                  L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",
                                  0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &hKey, nullptr);

    if (result == ERROR_SUCCESS) {
        DWORD value = 1; // 1 表示禁用 Windows 键
        result = RegSetValueExW(hKey, L"NoWinKeys", 0, REG_DWORD,
                                reinterpret_cast<const BYTE*>(&value), sizeof(value));

        if (result == ERROR_SUCCESS) {
            qDebug(log) << "注册表设置成功，Windows 键已禁用";
        } else {
            qWarning(log) << "注册表设置失败:" << result;
            success = false;
        }
        RegCloseKey(hKey);
    } else {
        qWarning(log) << "创建注册表键失败:" << result;
        success = false;
    }

    return success;
}

bool CHookWindows::EnableWindowsKey()
{
    bool success = true;

    // 取消注册热键
    UnregisterHotKey(nullptr, 1);
    UnregisterHotKey(nullptr, 2);

    // 恢复注册表设置
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER,
                                L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",
                                0, KEY_ALL_ACCESS, &hKey);

    if (result == ERROR_SUCCESS) {
        result = RegDeleteValueW(hKey, L"NoWinKeys");
        if (result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND) {
            qDebug(log) << "Windows 键已恢复";
        } else {
            qWarning(log) << "删除注册表值失败:" << result;
            success = false;
        }
        RegCloseKey(hKey);
    }

    return success;
}

bool CHookWindows::DisableTaskManager()
{
    bool bRet = false;
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER,
                                  L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
                                  0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &hKey, nullptr);

    if (result == ERROR_SUCCESS) {
        DWORD value = 1; // 1 表示禁用任务管理器
        result = RegSetValueExW(hKey, L"DisableTaskMgr", 0, REG_DWORD,
                                reinterpret_cast<const BYTE*>(&value), sizeof(value));

        if (result == ERROR_SUCCESS) {
            qDebug(log) << "任务管理器已禁用";
            bRet = true;
        } else {
            qWarning(log) << "禁用任务管理器失败:" << result;
        }
        RegCloseKey(hKey);
    }

    return bRet;
}

bool CHookWindows::EnableTaskManager()
{
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER,
                                L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System",
                                0, KEY_ALL_ACCESS, &hKey);

    if (result == ERROR_SUCCESS) {
        result = RegDeleteValueW(hKey, L"DisableTaskMgr");
        RegCloseKey(hKey);

        if (result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND) {
            qDebug(log) << "任务管理器已恢复";
            return true;
        }
    }

    return false;
}

