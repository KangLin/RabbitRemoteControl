#ifndef CHOOKWINDOWS_H
#define CHOOKWINDOWS_H

#include "Hook.h"
#if defined(Q_OS_WIN)
    #include "Windows.h"
#endif

class CHookWindows : public CHook
{
    Q_OBJECT
public:
    explicit CHookWindows(QObject *parent = nullptr);
    virtual ~CHookWindows();

public:
    virtual int RegisterKeyboard() override;
    virtual int UnRegisterKeyboard() override;

    // QAbstractNativeEventFilter interface
public:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;
#else
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
#endif

private:
    static LRESULT CALLBACK keyboardHookProc(INT code, WPARAM wparam, LPARAM lparam);
    HHOOK m_hKeyboard;
};

#endif // CHOOKWINDOWS_H
