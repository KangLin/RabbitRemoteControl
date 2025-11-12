// Author: Kang Lin <kl222@126.com>

#include "NativeEventFilterUnix.h"

#if defined(Q_OS_WIN)
#include <windows.h>
#endif

#include <QApplication>
#include <QLoggingCategory>
#include <QKeyEvent>
#include <QProcess>

static Q_LOGGING_CATEGORY(log, "Client.NativeEventFilter")

#include <xcb/xcb.h>
#include <xcb/xcb_keysyms.h>
#define XK_MISCELLANY
// the X11 headers on some systems. See: [Xlib 键符号](https://www.x.org/releases/X11R7.7/doc/libX11/XKB/xkblib.html)
#include <X11/keysymdef.h>

/*
int GetKeySym(xcb_key_press_event_t* event, xcb_keysym_t& keysym)
{
    xcb_connection_t *connection = xcb_connect(NULL, NULL);
    // 连接到 X server
    if (xcb_connection_has_error(connection)) {
        qCritical(log) << "Don't connect X server";
        return -1;
    }
    // 初始化 Key Symbols
    xcb_key_symbols_t *key_symbols = xcb_key_symbols_alloc(connection);
    if (!key_symbols) {
        qCritical(log) << "无法分配键符表";
        return -2;
    }
    // 将 keycode 转换为 keysym
    keysym = xcb_key_symbols_get_keysym(key_symbols, event->detail, 0);
    // 处理 Shift 组合键
    if (event->state & XCB_MOD_MASK_SHIFT) {
        keysym = xcb_key_symbols_get_keysym(key_symbols, event->detail, 1);
    }
    qDebug(log) << "keycode:" << event->detail << "keySym:" << keysym;
    // 清理
    xcb_key_symbols_free(key_symbols);
    xcb_disconnect(connection);
    return 0;
}

void
print_modifiers (uint32_t mask)
{
    const char **mod, *mods[] = {
                          "Shift", "Lock", "Ctrl", "Alt",
                          "Mod2", "Mod3", "Mod4", "Mod5",
                          "Button1", "Button2", "Button3", "Button4", "Button5"
                      };
    qDebug(log) << "Modifier mask: ";
    for (mod = mods ; mask; mask >>= 1, mod++)
        if (mask & 1)
            qDebug(log) << *mod;
}
*/

Qt::KeyboardModifiers GetModifiers(uint32_t mask)
{
    Qt::KeyboardModifiers modifiers = Qt::NoModifier;
    if(XCB_KEY_BUT_MASK_SHIFT & mask)
        modifiers |= Qt::ShiftModifier;
    if(XCB_KEY_BUT_MASK_CONTROL & mask)
        modifiers |= Qt::ControlModifier;
    if(XCB_KEY_BUT_MASK_MOD_1 & mask)
        modifiers |= Qt::AltModifier;
    return modifiers;
}

bool CNativeEventFilterUnix::HandleKey(
    xcb_keysym_t keysym, QEvent::Type type, Qt::KeyboardModifiers modifiers)
{
    int bRet = false;
    if(!m_pParameterPlugin
        || !m_pParameterPlugin->GetCaptureAllKeyboard()) {
        qDebug(log) << "Native window receive keyboard.";
        return false;
    }

    int key = 0;
    switch (keysym) {
    case XK_Tab:
        key = Qt::Key_Tab;
        bRet = true;
        break;
    case XK_Meta_L:
    case XK_Meta_R:
        key = Qt::Key_Meta;
        bRet = true;
        break;
    case XK_Alt_L:
    case XK_Alt_R:
        key = Qt::Key_Alt;
        bRet = true;
        break;
    case XK_Super_L:
        key = Qt::Key_Super_L;
        bRet = true;
        break;
    case XK_Super_R:
        key = Qt::Key_Super_R;
        bRet = true;
        break;
    default:
        break;
    }

    if(bRet) {
        CFrmViewer* focus = qobject_cast<CFrmViewer*>(QApplication::focusWidget());
        if(focus) {
            
            QKeyEvent* keyEvent = new QKeyEvent(type, key, modifiers);
            /*
                QApplication::postEvent(focus, keyEvent);
                return true;//*/
            switch(type)
            {
            case QEvent::KeyPress:
                emit focus->sigKeyPressEvent(keyEvent);
                break;
            case QEvent::KeyRelease:
                emit focus->sigKeyReleaseEvent(keyEvent);
                break;
            default:
                break;
            }
            // Because the signals is `Qt::DirectConnection`,
            // so than can delete it in here!
            // See: CBackendDesktop::SetViewer
            delete keyEvent;
            return true;
        }
        /*
        QKeyEvent* keyEvent = new QKeyEvent(type, key, modifiers);
        qDebug(log) << "Process:" << keyEvent;
        delete keyEvent;//*/
    }
    return false;
}

bool CNativeEventFilterUnix::HandleEvent(xcb_generic_event_t* event)
{
    bool bRet = false;

    switch (event->response_type & ~0x80) {
    case XCB_KEY_PRESS: {
        xcb_key_press_event_t *ke = (xcb_key_press_event_t *)event;
        //print_modifiers(ke->state);
        int nRet = 0;
        xcb_keysym_t keysym;
        nRet = GetKeySym(ke, keysym);
        if(nRet)
            break;
        /*
        qDebug(log) << "Press Key:" << ke->detail << keysym << GetModifiers(ke->state) << "pressed in window " << ke->event
                    << "root:" << ke->root;//*/
        bRet = HandleKey(keysym, QEvent::KeyPress, GetModifiers(ke->state));
        break;
    }
    case XCB_KEY_RELEASE: {
        xcb_key_release_event_t *ke = (xcb_key_release_event_t *)event;
        //print_modifiers(ke->state);
        int nRet = 0;
        xcb_keysym_t keysym;
        nRet = GetKeySym(ke, keysym);
        if(nRet)
            break;
        /*
        qDebug(log) << "Release Key:" << ke->detail << keysym << GetModifiers(ke->state) << "pressed in window " << ke->event
                    << "root:" << ke->root;//*/
        bRet = HandleKey(keysym, QEvent::KeyRelease, GetModifiers(ke->state));
        break;
    }
    default:
        break;
    }

    return bRet;
}

int CNativeEventFilterUnix::GetKeySym(xcb_key_press_event_t *event, xcb_keysym_t &keysym)
{
    int nRet = 0;
    // 将 keycode 转换为 keysym
    keysym = xcb_key_symbols_get_keysym(m_pKeySymbols, event->detail, 0);
    // 处理 Shift 组合键
    if (event->state & XCB_MOD_MASK_SHIFT) {
        keysym = xcb_key_symbols_get_keysym(m_pKeySymbols, event->detail, 1);
    }
    //qDebug(log) << "keycode:" << event->detail << "keySym:" << keysym;
    return nRet;
}

void CNativeEventFilterUnix::DisableSuperKeyShortcuts()
{
    // GNOME
    QProcess::execute("gsettings", {"set", "org.gnome.mutter", "overlay-key", ""});

    // KDE (需要检测桌面环境)
    QString desktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
    if (desktop.contains("KDE", Qt::CaseInsensitive)) {
        QProcess::execute("kwriteconfig5", {"--file", "kwinrc", "--group", "ModifierOnlyShortcuts", "--key", "Meta", ""});
        QProcess::execute("kwin_x11", {"--replace"});
    }
}

void CNativeEventFilterUnix::RestoreSuperKeyShortcuts()
{
    // GNOME
    QProcess::execute("gsettings", {"reset", "org.gnome.mutter", "overlay-key"});

    // KDE (需要检测桌面环境)
    QString desktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
    if (desktop.contains("KDE", Qt::CaseInsensitive)) {
        QProcess::execute("kwriteconfig5", {"--file", "kwinrc", "--group", "ModifierOnlyShortcuts", "--key", "Meta", "org.kde.kglobalaccel,/component/kwin,,invokeShortcut,Show Desktop Grid"});
        QProcess::execute("kwin_x11", {"--replace"});
    }
}

CNativeEventFilterUnix::CNativeEventFilterUnix(CParameterPlugin *pPara)
    : m_pParameterPlugin(pPara)
{
    m_DesktopShortcutManager.disableAllShortcuts();
    m_pConnect = xcb_connect(NULL, NULL);
    // 连接到 X server
    if (xcb_connection_has_error(m_pConnect)) {
        qCritical(log) << "Don't connect X server";
        return;
    }

    // 初始化 Key Symbols
    m_pKeySymbols = xcb_key_symbols_alloc(m_pConnect);
    if (!m_pKeySymbols) {
        qCritical(log) << "Unable to allocate symbol table";
        return;
    }
}

CNativeEventFilterUnix::~CNativeEventFilterUnix()
{
    // 清理
    if(m_pKeySymbols)
        xcb_key_symbols_free(m_pKeySymbols);
    xcb_disconnect(m_pConnect);
    m_DesktopShortcutManager.restoreAllShortcuts();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool CNativeEventFilterUnix::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
#else
bool CNativeEventFilterUnix::nativeEventFilter(const QByteArray &eventType, void *message, long *result)
#endif
{
    //qDebug(log) << "eventType:" << eventType;

    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* e = static_cast<xcb_generic_event_t *>(message);
        return HandleEvent(e);
    }
#if defined(Q_OS_WIN)
    if (eventType == "windows_generic_MSG" || eventType == "windows_dispatcher_MSG")
    {
        MSG *pMsg = reinterpret_cast<MSG *>(message);
                
    }
#endif
    return false;
}
