// Author: Kang Lin <kl222@126.com>

#ifndef KEYINJECTOR_H
#define KEYINJECTOR_H

#include <QObject>
#include <QList>
#include <QKeySequence>

class KeyInjector : public QObject
{
    Q_OBJECT
public:
    explicit KeyInjector(QObject *parent = nullptr);
    ~KeyInjector();

    // 必须先调用 init() 以选择后端（X11 或 uinput）
    bool init();

    // 发送单个按键（按下并释放）
    // key: Qt::Key value (e.g., Qt::Key_Super_L, Qt::Key_R)
    bool sendKey(Qt::Key key);

    // 发送组合键（例如 Super + R）
    // modifiers: list of modifier keys (e.g., Qt::Key_Super_L)
    // key: 主键
    bool sendKeyCombo(const QList<Qt::Key> &modifiers, Qt::Key key);

    // 直接发送 Super 左键（便捷）
    bool sendSuper();

    // 清理（销毁虚拟设备等）
    void cleanup();

private:
    bool initX11();
    bool initUInput();
    void cleanupX11();
    void cleanupUInput();

    bool sendKeyX11(Qt::Key key, bool press);
    bool sendKeyUInput(int linux_keycode, int value);

    int qtKeyToKeySym(Qt::Key key, bool *ok = nullptr);    // X11 keysym (XK_*), returns 0 on fail
    int qtKeyToLinuxKey(Qt::Key key, bool *ok = nullptr);  // Linux KEY_* code, returns 0 on fail

    bool useX11 = false;
    bool useUinput = false;
    void *x11Display = nullptr; // opaque pointer to Display*
    int uinputFd = -1;
};

#endif // KEYINJECTOR_H
