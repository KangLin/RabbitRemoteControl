// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>

class CParameterPlugin;
/*!
 * \brief Desktop manager
 * \see CDesktopManagerThread
 */
class CDesktopManager : public QObject
{
    Q_OBJECT

public:
    explicit CDesktopManager(
        CParameterPlugin* para, QObject *parent = nullptr);
    virtual ~CDesktopManager() override;

    //! Synchronous running program
    static bool RunCommand(const QString &program,
                           const QStringList &args = QStringList(),
                           int timeout = 5000);
    //! Synchronous running program. and return the program output.
    static QString GetCommandOutput(const QString &program,
                                    const QStringList &args = QStringList(),
                                    int timeout = 5000);

public Q_SLOTS:
    //! Disable shortcuts
    virtual void slotDisableShortcuts();
    //! Restore shortcuts
    virtual void slotRestoreShortcuts();

protected:
    //! Disable shortcuts
    virtual bool OnDisableShortcuts() = 0;
    //! Restore shortcuts
    virtual bool OnRestoreShortcuts() = 0;

protected:
    CParameterPlugin* m_pParameterPlugin;
    bool m_bScript;
};
