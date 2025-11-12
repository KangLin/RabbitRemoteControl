// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include "ParameterPlugin.h"
#include "Unix/DesktopShortcuts.h"

/*!
 * \brief The class is the HOOK abstract class.
 *        Call CHook::GetHook get a instance of the class.
 * \note The class is only used by Client.
 */
class CHook : public QObject
{
public:
    static CHook* GetHook(CParameterPlugin* pPara, QObject *parent = nullptr);

    virtual int RegisterKeyboard();
    virtual int UnRegisterKeyboard();

    static bool RunCommand(const QString &program, const QStringList &args = QStringList(), int timeout = 5000);

public:
    // QObject interface
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

protected:
    CParameterPlugin* m_pParameterPlugin;

protected:
    explicit CHook(CParameterPlugin* pPara, QObject *parent = nullptr);
    virtual ~CHook();

    virtual int OnRegisterKeyboard();
    virtual int OnUnRegisterKeyboard();
    virtual int OnDisableDesktopShortcuts();
    virtual int OnRestoreDesktopShortcuts();

private:
    bool m_bScript;
    CDesktopShortcutManager m_DesktopShortcutsManager;
};

