// Author: Kang Lin <kl222@126.com>

#ifndef CHOOKWINDOWS_H
#define CHOOKWINDOWS_H

#include "Hook.h"
#include "Windows.h"

class CHookWindows : public CHook
{
    Q_OBJECT
public:
    explicit CHookWindows(CParameterPlugin* pParaClient, QObject *parent = nullptr);
    virtual ~CHookWindows();

protected:
    virtual int OnRegisterKeyboard() override;
    virtual int OnUnRegisterKeyboard() override;
    virtual int OnDisableDesktopShortcuts() override;
    virtual int OnRestoreDesktopShortcuts() override;

private:
    static LRESULT CALLBACK keyboardHookProc(INT code, WPARAM wparam, LPARAM lparam);
    HHOOK m_hKeyboard;

    void DisableTaskManager(bool flag);
    bool DisableWindowsKey();
    bool EnableWindowsKey();
    bool DisableTaskManager();
    bool EnableTaskManager();
};

#endif // CHOOKWINDOWS_H
