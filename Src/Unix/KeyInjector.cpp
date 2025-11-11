// Author: Kang Lin <kl222@126.com>

#include "KeyInjector.h"
#include <QCoreApplication>
#include <QDebug>
#include <QProcessEnvironment>

#ifdef Q_OS_UNIX
// X11
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#endif

#ifdef __linux__
#include <fcntl.h>
#include <unistd.h>
#include <linux/uinput.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/time.h>
#endif

KeyInjector::KeyInjector(QObject *parent)
    : QObject(parent)
{
}

KeyInjector::~KeyInjector()
{
    cleanup();
}

bool KeyInjector::init()
{
    // 先检测 DISPLAY / WAYLAND 环境，优先 X11/XTest（无 root），否则尝试 uinput
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    bool hasDisplay = env.contains("DISPLAY");
    bool hasWayland = env.contains("WAYLAND_DISPLAY");

#ifdef Q_OS_UNIX
    if (hasDisplay) {
        if (initX11()) {
            useX11 = true;
            useUinput = false;
            qDebug() << "KeyInjector: using X11/XTest backend";
            return true;
        } else {
            qDebug() << "KeyInjector: X11 init failed";
        }
    }
#endif

#ifdef __linux__
    // 尝试 uinput（需要权限）
    if (initUInput()) {
        useUinput = true;
        useX11 = false;
        qDebug() << "KeyInjector: using uinput backend";
        return true;
    } else {
        qDebug() << "KeyInjector: uinput init failed or no permission";
    }
#endif

    qWarning() << "KeyInjector: no available backend for injecting keys";
    return false;
}

bool KeyInjector::initX11()
{
#ifdef Q_OS_UNIX
    // 使用 XOpenDisplay 以减少对 QtX11Extras 的依赖
    const char *displayName = getenv("DISPLAY");
    Display *dpy = nullptr;
    if (displayName)
        dpy = XOpenDisplay(displayName);
    else
        dpy = XOpenDisplay(nullptr);

    if (!dpy) {
        qDebug() << "KeyInjector: XOpenDisplay failed";
        return false;
    }

    // 测试 XTest 扩展是否可用
    int event_base, error_base;
    if (!XTestQueryExtension(dpy, &event_base, &error_base, &event_base, &error_base)) {
        XCloseDisplay(dpy);
        qDebug() << "KeyInjector: XTest extension not available";
        return false;
    }

    x11Display = dpy;
    return true;
#else
    Q_UNUSED(x11Display);
    return false;
#endif
}

void KeyInjector::cleanupX11()
{
#ifdef Q_OS_UNIX
    if (x11Display) {
        XCloseDisplay(static_cast<Display*>(x11Display));
        x11Display = nullptr;
    }
#endif
}

bool KeyInjector::initUInput()
{
#ifdef __linux__
    const char *device = "/dev/uinput";
    int fd = open(device, O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        qDebug() << "KeyInjector: open /dev/uinput failed, maybe permission denied";
        return false;
    }

    // 申明事件类型和按键
    ioctl(fd, UI_SET_EVBIT, EV_KEY);

    // 注册常用按键（Super / letters / digits / modifiers），可按需扩展
    ioctl(fd, UI_SET_KEYBIT, KEY_LEFTMETA);
    ioctl(fd, UI_SET_KEYBIT, KEY_RIGHTMETA);
    ioctl(fd, UI_SET_KEYBIT, KEY_LEFTSHIFT);
    ioctl(fd, UI_SET_KEYBIT, KEY_RIGHTSHIFT);
    ioctl(fd, UI_SET_KEYBIT, KEY_LEFTCTRL);
    ioctl(fd, UI_SET_KEYBIT, KEY_RIGHTCTRL);
    ioctl(fd, UI_SET_KEYBIT, KEY_LEFTALT);
    ioctl(fd, UI_SET_KEYBIT, KEY_RIGHTALT);

    // 注册字母与数字的一小部分（a-z, 0-9）
    for (int c = KEY_A; c <= KEY_Z; ++c) ioctl(fd, UI_SET_KEYBIT, c);
    for (int c = KEY_1; c <= KEY_9; ++c) ioctl(fd, UI_SET_KEYBIT, c);
    ioctl(fd, UI_SET_KEYBIT, KEY_0);

    struct uinput_user_dev uidev;
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, sizeof(uidev.name), "rabbitremote-qt-uinput");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1234;
    uidev.id.product = 0x5678;
    write(fd, &uidev, sizeof(uidev));

    if (ioctl(fd, UI_DEV_CREATE) < 0) {
        close(fd);
        qDebug() << "KeyInjector: UI_DEV_CREATE failed";
        return false;
    }

    // 保存 fd
    uinputFd = fd;
    // 小延时等待设备创建
    sleep(1);
    return true;
#else
    Q_UNUSED(uinputFd);
    return false;
#endif
}

void KeyInjector::cleanupUInput()
{
#ifdef __linux__
    if (uinputFd >= 0) {
        ioctl(uinputFd, UI_DEV_DESTROY);
        close(uinputFd);
        uinputFd = -1;
    }
#endif
}

void KeyInjector::cleanup()
{
    if (useX11) cleanupX11();
    if (useUinput) cleanupUInput();
    useX11 = false;
    useUinput = false;
}

bool KeyInjector::sendKey(Qt::Key key)
{
    return sendKeyCombo({}, key);
}

bool KeyInjector::sendKeyCombo(const QList<Qt::Key> &modifiers, Qt::Key key)
{
    bool ok;
    // 按下修饰键
    for (Qt::Key mod : modifiers) {
        if (useX11) {
            sendKeyX11(mod, true);
        } else if (useUinput) {
            int lk = qtKeyToLinuxKey(mod, &ok);
            if (ok) sendKeyUInput(lk, 1);
        }
    }

    // 按下主键
    if (useX11) {
        sendKeyX11(key, true);
        // 释放主键
        sendKeyX11(key, false);
    } else if (useUinput) {
        int lk = qtKeyToLinuxKey(key, &ok);
        if (ok) {
            sendKeyUInput(lk, 1);
            sendKeyUInput(lk, 0);
        } else {
            qDebug() << "KeyInjector: unknown linux key for key" << key;
        }
    }

    // 释放修饰键（倒序以模拟真实顺序）
    for (int i = modifiers.size()-1; i >= 0; --i) {
        Qt::Key mod = modifiers.at(i);
        if (useX11) {
            sendKeyX11(mod, false);
        } else if (useUinput) {
            int lk = qtKeyToLinuxKey(mod, &ok);
            if (ok) sendKeyUInput(lk, 0);
        }
    }

    return true;
}

bool KeyInjector::sendSuper()
{
    return sendKey(Qt::Key_Super_L);
}

bool KeyInjector::sendKeyX11(Qt::Key key, bool press)
{
#ifdef Q_OS_UNIX
    if (!x11Display) return false;
    Display *dpy = static_cast<Display*>(x11Display);

    bool ok;
    int ks = qtKeyToKeySym(key, &ok);
    if (!ok) return false;

    KeyCode kc = XKeysymToKeycode(dpy, (KeySym)ks);
    if (kc == 0) {
        qDebug() << "KeyInjector: XKeysymToKeycode failed for keysym" << ks;
        return false;
    }

    XTestFakeKeyEvent(dpy, kc, press ? True : False, CurrentTime);
    XFlush(dpy);
    return true;
#else
    Q_UNUSED(key);
    Q_UNUSED(press);
    return false;
#endif
}

bool KeyInjector::sendKeyUInput(int linux_keycode, int value)
{
#ifdef __linux__
    if (uinputFd < 0) return false;
    struct input_event ev;
    memset(&ev, 0, sizeof(ev));
    gettimeofday(&ev.time, NULL);
    ev.type = EV_KEY;
    ev.code = linux_keycode;
    ev.value = value; // 1 down, 0 up
    if (write(uinputFd, &ev, sizeof(ev)) < 0) {
        qDebug() << "KeyInjector: write key event failed";
        return false;
    }
    // sync
    memset(&ev, 0, sizeof(ev));
    gettimeofday(&ev.time, NULL);
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ev.value = 0;
    write(uinputFd, &ev, sizeof(ev));
    return true;
#else
    Q_UNUSED(linux_keycode);
    Q_UNUSED(value);
    return false;
#endif
}

// 简单映射函数，覆盖常见键（可扩展）
// 返回 X11 keysym（例如 XK_Super_L）或 0
int KeyInjector::qtKeyToKeySym(Qt::Key key, bool *ok)
{
    if (ok) *ok = false;
    switch (key) {
    case Qt::Key_Super_L:
        if (ok) *ok = true;
        return XK_Super_L;
    case Qt::Key_Super_R:
        if (ok) *ok = true;
        return XK_Super_R;
    case Qt::Key_Shift:
    //case Qt::Key_ShiftL:
        if (ok) *ok = true;
        return XK_Shift_L;
    case Qt::Key_Control:
    //case Qt::Key_ControlL:
        if (ok) *ok = true;
        return XK_Control_L;
    case Qt::Key_Alt:
    //case Qt::Key_AltL:
        if (ok) *ok = true;
        return XK_Alt_L;
    default:
        // 字母
        if (key >= Qt::Key_A && key <= Qt::Key_Z) {
            if (ok) *ok = true;
            QChar ch = QChar('a' + (key - Qt::Key_A));
            return XStringToKeysym(QString(ch).toLatin1().constData());
        }
        // 数字
        if (key >= Qt::Key_0 && key <= Qt::Key_9) {
            if (ok) *ok = true;
            QChar ch = QChar('0' + (key - Qt::Key_0));
            return XStringToKeysym(QString(ch).toLatin1().constData());
        }
    }
    return 0;
}

// 返回 Linux input keycode (KEY_*)，0 表示失败
int KeyInjector::qtKeyToLinuxKey(Qt::Key key, bool *ok)
{
    if (ok) *ok = false;
    switch (key) {
    case Qt::Key_Super_L:
        if (ok) *ok = true;
        return KEY_LEFTMETA;
    case Qt::Key_Super_R:
        if (ok) *ok = true;
        return KEY_RIGHTMETA;
    case Qt::Key_Shift:
    //case Qt::Key_ShiftL:
        if (ok) *ok = true;
        return KEY_LEFTSHIFT;
    case Qt::Key_Control:
    //case Qt::Key_ControlL:
        if (ok) *ok = true;
        return KEY_LEFTCTRL;
    case Qt::Key_Alt:
    //case Qt::Key_AltL:
        if (ok) *ok = true;
        return KEY_LEFTALT;
    default:
        if (key >= Qt::Key_A && key <= Qt::Key_Z) {
            if (ok) *ok = true;
            return KEY_A + (key - Qt::Key_A);
        }
        if (key >= Qt::Key_0 && key <= Qt::Key_9) {
            if (ok) *ok = true;
            int offset = (key == Qt::Key_0) ? 9 : (key - Qt::Key_1);
            // 按 linux headers, KEY_1 .. KEY_9, KEY_0
            if (key == Qt::Key_0) return KEY_0;
            return KEY_1 + (key - Qt::Key_1);
        }
    }
    return 0;
}
