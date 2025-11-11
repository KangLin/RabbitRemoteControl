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

static Q_LOGGING_CATEGORY(log, "Client.Hook.Windows")

CHookWindows::CHookWindows(CParameterClient *pParaClient, QObject *parent)
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
int CHookWindows::RegisterKeyboard()
{
    if(!RabbitCommon::CTools::Instance()->HasAdministratorPrivilege()
        && m_pParameterClient->GetPromptAdministratorPrivilege())
    {
        int nRet = 0;
        QMessageBox msg(
            QMessageBox::Warning, tr("Warning"),
            tr("The programe is not administrator privilege.\n"
               "Don't disable system shortcuts(eg: Ctrl+Alt+del).\n"
               "Restart program by administrator?"),
            QMessageBox::Yes | QMessageBox::No);
        msg.setCheckBox(new QCheckBox(tr("Always shown"), &msg));
        msg.checkBox()->setCheckable(true);
        msg.checkBox()->setChecked(
            m_pParameterClient->GetPromptAdministratorPrivilege());
        nRet = msg.exec();
        if(QMessageBox::Yes == nRet) {
            RabbitCommon::CTools::Instance()->StartWithAdministratorPrivilege(true);
        }
        if(m_pParameterClient->GetPromptAdministratorPrivilege()
            != msg.checkBox()->isChecked()) {
            m_pParameterClient->SetPromptAdministratorPrivilege(
                msg.checkBox()->isChecked());
        }
    }

    if(m_hKeyboard)
        UnRegisterKeyboard();
    DisableTaskManager(true);
    m_hKeyboard = SetWindowsHookExW(WH_KEYBOARD_LL, keyboardHookProc, nullptr, 0);
    if(NULL == m_hKeyboard) {
        qCritical(log) << "SetWindowsHookExW error:" << GetLastError();
        return -1;
    }
    return 0;
}

int CHookWindows::UnRegisterKeyboard()
{
    if(m_hKeyboard)
    {
        UnhookWindowsHookEx(m_hKeyboard);
        m_hKeyboard = nullptr;
    }
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
void CHookWindows::DisableTaskManager(bool flag)
{
    // 屏蔽ctrl + alt +del 需要修改注册表的值， 取得管理员权限， 关闭360等杀毒软件
    int value = flag ? 0x00000001 : 0x00000000;
    QSettings settings(TASKMANAGERSystem, QSettings::NativeFormat);
    settings.setValue("DisableTaskMgr", value); //任务管理器
    settings.setValue("DisableChangePassword", value); //更改密码
    settings.setValue("DisableLockWorkstation", value); //锁定计算机
    settings.setValue("DisableSwitchUserOption", value); //切换用户

    QSettings settings2(TASKMANAGERExplorer, QSettings::NativeFormat);
    settings2.setValue("NoLogOff", value); //注销
}
