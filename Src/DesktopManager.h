// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QThread>
class CParameterPlugin;
class CDesktopManager : public QThread
{
    Q_OBJECT

public:
    explicit CDesktopManager(
        CParameterPlugin* para, QObject *parent = nullptr);
    virtual ~CDesktopManager();

    static bool RunCommand(const QString &program,
                           const QStringList &args = QStringList(),
                           int timeout = 5000);
    static QString GetCommandOutput(const QString &program,
                                    const QStringList &args = QStringList(),
                                    int timeout = 5000);

public Q_SLOTS:
    virtual void slotDisableShortcuts();
    virtual void slotRestoreShortcuts();

protected:
    virtual bool OnDisableShortcuts() = 0;
    virtual bool OnRestoreShortcuts() = 0;

protected:
    CParameterPlugin* m_pParameterPlugin;
    bool m_bScript;
};
