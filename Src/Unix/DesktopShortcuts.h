// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QString>
#include <QProcess>
#include <QMap>
#include <QSettings>
#include <QFile>
#include <QDir>

class CDesktopShortcutManager : public QObject
{
    Q_OBJECT

public:
    explicit CDesktopShortcutManager(QObject *parent = nullptr);
    virtual ~CDesktopShortcutManager();

    // 主要功能
    bool disableAllShortcuts();
    bool restoreAllShortcuts();
    
    // 状态查询
    bool isDisabled() const { return m_shortcutsDisabled; }
    QString desktopEnvironment() const { return m_desktopEnv; }

    // 检测方法
    static QString detectDesktopEnvironment();
    static bool runCommand(const QString &program, const QStringList &args = QStringList(), int timeout = 5000);
    static QString getCommandOutput(const QString &program, const QStringList &args);

private:
    QString m_desktopEnv;
    bool m_shortcutsDisabled = false;
    
    // 存储原始设置
    QMap<QString, QVariant> m_gnomeSettings;
    QMap<QString, QString> m_kdeSettings;
    QString m_gnomeBackupPath;
    QString m_kdeBackupPath;

#if defined(Q_OS_LINUX)
    // GNOME 方法
    bool disableGNOMEShortcuts();
    bool restoreGNOMEShortcuts();
    bool resetGNOMEShortcuts();
    void backupGNOMESettings();
    
    // KDE 方法
    bool disableKDEShortcuts();
    bool restoreKDEShortcuts();
    void backupKDESettings();
#endif

    // 通用方法
    bool backupFile(const QString &sourcePath, const QString &backupPath);
    bool restoreFile(const QString &backupPath, const QString &targetPath);
};
