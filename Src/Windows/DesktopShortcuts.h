// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QHash>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class CDesktopShortcutsManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit CDesktopShortcutsManager(QObject *parent = nullptr);
    ~CDesktopShortcutsManager();

    // 快捷键管理
    bool disableAllHotkeys();
    bool restoreAllHotkeys();
    bool isDisabled() const { return m_hotkeysDisabled; }

    // 特定快捷键控制
    bool disableWindowsKey();
    bool enableWindowsKey();
    bool disableAltTab();
    bool enableAltTab();
    bool disableTaskManager();
    bool enableTaskManager();

    // QAbstractNativeEventFilter 接口
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

signals:
    void hotkeyPressed(int keyCode, const QString &keyName);
    void hotkeyBlocked(int keyCode, const QString &keyName);
    void statusChanged(bool disabled);

private:
    void initializeKeys();
    bool setKeyboardHook();
    bool removeKeyboardHook();
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

private:
    bool m_hotkeysDisabled = false;
    bool m_keyboardHooked = false;
    
#ifdef Q_OS_WIN
    HHOOK m_keyboardHook = nullptr;
    QHash<int, QString> m_keyNames;
    
    // 存储原始注册表值
    QHash<QString, QVariant> m_registryBackup;
#endif
};
