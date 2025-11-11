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

public:
    virtual int RegisterKeyboard() override;
    virtual int UnRegisterKeyboard() override;

private:
    static LRESULT CALLBACK keyboardHookProc(INT code, WPARAM wparam, LPARAM lparam);
    HHOOK m_hKeyboard;

    void DisableTaskManager(bool flag);
};

#endif // CHOOKWINDOWS_H
