// windowshotkeymanager.cpp

#include <QLoggingCategory>
#include <QCoreApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#include <winuser.h>
#endif

#include "DesktopShortcuts.h"
static Q_LOGGING_CATEGORY(log, "Plugin.Hook.Desktop.Shortcut")
CDesktopShortcutsManager::CDesktopShortcutsManager(QObject *parent)
    : QObject(parent)
{
    initializeKeys();
    
    // 安装原生事件过滤器
    qApp->installNativeEventFilter(this);
}

CDesktopShortcutsManager::~CDesktopShortcutsManager()
{
    restoreAllHotkeys();
    removeKeyboardHook();
}

void CDesktopShortcutsManager::initializeKeys()
{
#ifdef Q_OS_WIN
    m_keyNames[VK_LWIN] = "Left Windows";
    m_keyNames[VK_RWIN] = "Right Windows";
    m_keyNames[VK_TAB] = "Tab";
    m_keyNames[VK_ESCAPE] = "Escape";
    m_keyNames[VK_CONTROL] = "Control";
    m_keyNames[VK_MENU] = "Alt";  // Alt 键的虚拟键码是 VK_MENU
    m_keyNames[VK_SHIFT] = "Shift";
    m_keyNames[VK_SPACE] = "Space";
    m_keyNames[VK_RETURN] = "Enter";
    m_keyNames[VK_BACK] = "Backspace";
    m_keyNames[VK_DELETE] = "Delete";
    m_keyNames[VK_UP] = "Up Arrow";
    m_keyNames[VK_DOWN] = "Down Arrow";
    m_keyNames[VK_LEFT] = "Left Arrow";
    m_keyNames[VK_RIGHT] = "Right Arrow";
    m_keyNames[VK_F1] = "F1";
    m_keyNames[VK_F2] = "F2";
    m_keyNames[VK_F3] = "F3";
    m_keyNames[VK_F4] = "F4";
    m_keyNames[VK_F5] = "F5";
    m_keyNames[VK_F6] = "F6";
    m_keyNames[VK_F7] = "F7";
    m_keyNames[VK_F8] = "F8";
    m_keyNames[VK_F9] = "F9";
    m_keyNames[VK_F10] = "F10";
    m_keyNames[VK_F11] = "F11";
    m_keyNames[VK_F12] = "F12";
#endif
}

bool CDesktopShortcutsManager::disableAllHotkeys()
{
    if (m_hotkeysDisabled) {
        return true;
    }

    bool success = true;
    
    // 禁用 Windows 键
    if (!disableWindowsKey()) {
        success = false;
        qWarning() << "禁用 Windows 键失败";
    }
    
    // 禁用 Alt+Tab
    if (!disableAltTab()) {
        success = false;
        qWarning() << "禁用 Alt+Tab 失败";
    }
    
    // 设置键盘钩子
    if (!setKeyboardHook()) {
        success = false;
        qWarning() << "设置键盘钩子失败";
    }
    
    if (success) {
        m_hotkeysDisabled = true;
        emit statusChanged(true);
        qDebug() << "所有 Windows 快捷键已禁用";
    }
    
    return success;
}

bool CDesktopShortcutsManager::restoreAllHotkeys()
{
    if (!m_hotkeysDisabled) {
        return true;
    }

    bool success = true;
    
    // 恢复 Windows 键
    if (!enableWindowsKey()) {
        success = false;
    }
    
    // 恢复 Alt+Tab
    if (!enableAltTab()) {
        success = false;
    }
    
    // 恢复任务管理器
    if (!enableTaskManager()) {
        success = false;
    }
    
    // 移除键盘钩子
    if (!removeKeyboardHook()) {
        success = false;
    }
    
    if (success) {
        m_hotkeysDisabled = false;
        emit statusChanged(false);
        qDebug() << "所有 Windows 快捷键已恢复";
    }
    
    return success;
}

bool CDesktopShortcutsManager::disableWindowsKey()
{
    // 方法1: 使用 RegisterHotKey 来占用 Windows 键
    bool success = true;
    
    // 注册热键来拦截 Windows 键
    if (!RegisterHotKey(nullptr, 1, MOD_WIN, VK_LWIN)) {
        qWarning() << "注册左 Windows 键拦截失败:" << GetLastError();
        success = false;
    }
    
    if (!RegisterHotKey(nullptr, 2, MOD_WIN, VK_RWIN)) {
        qWarning() << "注册右 Windows 键拦截失败:" << GetLastError();
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
            qDebug() << "注册表设置成功，Windows 键已禁用";
        } else {
            qWarning() << "注册表设置失败:" << result;
            success = false;
        }
        RegCloseKey(hKey);
    } else {
        qWarning() << "创建注册表键失败:" << result;
        success = false;
    }
    
    return success;
}

bool CDesktopShortcutsManager::enableWindowsKey()
{
#ifdef Q_OS_WIN
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
            qDebug() << "Windows 键已恢复";
        } else {
            qWarning() << "删除注册表值失败:" << result;
            success = false;
        }
        RegCloseKey(hKey);
    }
    
    return success;
#else
    return false;
#endif
}

bool CDesktopShortcutsManager::disableAltTab()
{
    /*
#ifdef Q_OS_WIN
    // 使用系统参数设置禁用 Alt+Tab
    BOOL result = SystemParametersInfo(SPI_SETSWITCHTASKS, 0, nullptr, 0);
    if (result) {
        qDebug() << "Alt+Tab 已禁用";
        return true;
    } else {
        qWarning() << "禁用 Alt+Tab 失败:" << GetLastError();
        return false;
    }
#else
    return false;
#endif //*/
    return false;
}

bool CDesktopShortcutsManager::enableAltTab()
{
    /*
#ifdef Q_OS_WIN
    BOOL result = SystemParametersInfo(SPI_SETSWITCHTASKS, 1, nullptr, 0);
    if (result) {
        qDebug() << "Alt+Tab 已恢复";
        return true;
    } else {
        qWarning() << "恢复 Alt+Tab 失败:" << GetLastError();
        return false;
    }
#else
    return false;
#endif//*/
    return false;
}

bool CDesktopShortcutsManager::disableTaskManager()
{
#ifdef Q_OS_WIN
    HKEY hKey;
    LONG result = RegCreateKeyExW(HKEY_CURRENT_USER, 
                                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 
                                 0, nullptr, 0, KEY_ALL_ACCESS, nullptr, &hKey, nullptr);
    
    if (result == ERROR_SUCCESS) {
        DWORD value = 1; // 1 表示禁用任务管理器
        result = RegSetValueExW(hKey, L"DisableTaskMgr", 0, REG_DWORD, 
                               reinterpret_cast<const BYTE*>(&value), sizeof(value));
        
        if (result == ERROR_SUCCESS) {
            qDebug() << "任务管理器已禁用";
            return true;
        } else {
            qWarning() << "禁用任务管理器失败:" << result;
        }
        RegCloseKey(hKey);
    }
#endif
    return false;
}

bool CDesktopShortcutsManager::enableTaskManager()
{
#ifdef Q_OS_WIN
    HKEY hKey;
    LONG result = RegOpenKeyExW(HKEY_CURRENT_USER, 
                               L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System", 
                               0, KEY_ALL_ACCESS, &hKey);
    
    if (result == ERROR_SUCCESS) {
        result = RegDeleteValueW(hKey, L"DisableTaskMgr");
        RegCloseKey(hKey);
        
        if (result == ERROR_SUCCESS || result == ERROR_FILE_NOT_FOUND) {
            qDebug() << "任务管理器已恢复";
            return true;
        }
    }
#endif
    return false;
}

bool CDesktopShortcutsManager::setKeyboardHook()
{
#ifdef Q_OS_WIN
    if (m_keyboardHooked) {
        return true;
    }
    
    m_keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, GetModuleHandle(nullptr), 0);
    if (m_keyboardHook) {
        m_keyboardHooked = true;
        qDebug() << "键盘钩子设置成功";
        return true;
    } else {
        qWarning() << "设置键盘钩子失败:" << GetLastError();
        return false;
    }
#else
    return false;
#endif
}

bool CDesktopShortcutsManager::removeKeyboardHook()
{
#ifdef Q_OS_WIN
    if (m_keyboardHook) {
        BOOL result = UnhookWindowsHookEx(m_keyboardHook);
        if (result) {
            m_keyboardHook = nullptr;
            m_keyboardHooked = false;
            qDebug() << "键盘钩子已移除";
            return true;
        } else {
            qWarning() << "移除键盘钩子失败:" << GetLastError();
            return false;
        }
    }
#endif
    return true;
}

#ifdef Q_OS_WIN
LRESULT CALLBACK CDesktopShortcutsManager::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT *kbStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        
        // 检查按键按下事件
        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
            int keyCode = kbStruct->vkCode;
            
            // 拦截系统快捷键
            if (keyCode == VK_LWIN || keyCode == VK_RWIN) {
                qDebug() << "拦截 Windows 键";
                return 1; // 阻止按键传递
            }
            
            // 拦截 Alt+Tab
            if (keyCode == VK_TAB && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
                qDebug() << "拦截 Alt+Tab";
                return 1;
            }
            
            // 拦截 Alt+Esc
            if (keyCode == VK_ESCAPE && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
                qDebug() << "拦截 Alt+Esc";
                return 1;
            }
            
            // 拦截 Ctrl+Esc (开始菜单)
            if (keyCode == VK_ESCAPE && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                qDebug() << "拦截 Ctrl+Esc";
                return 1;
            }
        }
    }
    
    // 继续传递消息
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
#endif

bool CDesktopShortcutsManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG") {
        MSG *msg = static_cast<MSG*>(message);
        
        // 处理热键消息
        if (msg->message == WM_HOTKEY) {
            int hotkeyId = static_cast<int>(msg->wParam);
            QString keyName = m_keyNames.value(hotkeyId, "Unknown");
            
            emit hotkeyBlocked(hotkeyId, keyName);
            qDebug() << "拦截系统热键:" << keyName;
            
            *result = 0;
            return true;
        }
        
        // 处理其他系统消息
        if (msg->message == WM_SYSCOMMAND) {
            // 拦截系统命令
            switch (msg->wParam) {
            case SC_SCREENSAVE:     // 屏幕保护
            case SC_MONITORPOWER:   // 显示器电源
            case SC_TASKLIST:       // 任务列表 (Ctrl+Esc)
                qDebug() << "拦截系统命令:" << msg->wParam;
                *result = 0;
                return true;
            }
        }
    }
#endif
    
    return false;
}
