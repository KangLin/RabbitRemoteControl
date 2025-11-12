// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QStandardPaths>
#include <QTemporaryDir>
#include "DesktopShortcuts.h"

static Q_LOGGING_CATEGORY(log, "Plugin.Hook.Desktop.Shortcut")

CDesktopShortcutManager::CDesktopShortcutManager(QObject *parent)
    : QObject(parent)
{
    m_desktopEnv = detectDesktopEnvironment();
    qDebug(log) << "Desktop environment:" << m_desktopEnv;
    
    // 设置备份路径
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    m_gnomeBackupPath = tempDir + "/gnome_shortcuts_backup.conf";
    m_kdeBackupPath = tempDir + "/kde_shortcuts_backup.conf";
}

CDesktopShortcutManager::~CDesktopShortcutManager()
{
    // 确保退出时恢复快捷键
    if (m_shortcutsDisabled) {
        qWarning(log) << "Shortcuts is disabled, restoring ......";
        restoreAllShortcuts();
    }
}

QString CDesktopShortcutManager::detectDesktopEnvironment()
{
    QString xdgDesktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP", "").toLower();
    QString desktopSession = qEnvironmentVariable("DESKTOP_SESSION", "").toLower();
    QString gdmSession = qEnvironmentVariable("GDMSESSION", "").toLower();

    if (xdgDesktop.contains("gnome") || desktopSession.contains("gnome") || gdmSession.contains("gnome")) {
        return "GNOME";
    } else if (xdgDesktop.contains("kde") || desktopSession.contains("plasma") || gdmSession.contains("plasma")) {
        return "KDE";
    } else if (xdgDesktop.contains("xfce") || desktopSession.contains("xfce")) {
        return "XFCE";
    } else if (xdgDesktop.contains("mate") || desktopSession.contains("mate")) {
        return "MATE";
    } else if (xdgDesktop.contains("cinnamon") || desktopSession.contains("cinnamon")) {
        return "Cinnamon";
    } else if (xdgDesktop.contains("pantheon") || desktopSession.contains("pantheon")) {
        return "Pantheon";
    } else {
        return "Unknown";
    }
}

bool CDesktopShortcutManager::disableAllShortcuts()
{
    if (m_shortcutsDisabled) {
        qDebug(log) << "Shortcuts is disabled";
        return true;
    }

    m_shortcutsDisabled = true;
    bool success = false;

#if defined(Q_OS_LINUX)
    if (m_desktopEnv == "GNOME") {
        success = disableGNOMEShortcuts();
    } else if (m_desktopEnv == "KDE") {
        success = disableKDEShortcuts();
    } else if (m_desktopEnv == "XFCE") {
        // XFCE 实现类似
        qWarning(log) << "XFCE Shortcut key disabling is not yet implemented";
    } else {
        qWarning(log) << "Unsupported desktop environment:" << m_desktopEnv;
        return false;
    }
#endif

    if (success) {
        qDebug(log) << "All desktop shortcuts have been disabled";
    }

    return success;
}

bool CDesktopShortcutManager::restoreAllShortcuts()
{
    if (!m_shortcutsDisabled) {
        qDebug(log) << "The shortcut key is not disabled, no need to restore.";
        return true;
    }

    m_shortcutsDisabled = false;
    bool success = false;

#if defined(Q_OS_LINUX)
    if (m_desktopEnv == "GNOME") {
        success = restoreGNOMEShortcuts();
        //resetGNOMEShortcuts();
    } else if (m_desktopEnv == "KDE") {
        success = restoreKDEShortcuts();
    } else {
        qWarning(log) << "Unsupported desktop environment:" << m_desktopEnv;
        return false;
    }
#endif

    if (success) {
        qDebug(log) << "All desktop shortcuts have been restored";
    }

    return success;
}

#if defined(Q_OS_LINUX)
// GNOME 快捷键管理
bool CDesktopShortcutManager::disableGNOMEShortcuts()
{
    qDebug(log) << "Disable GNOME shortcuts ......";

    // 备份当前设置
    backupGNOMESettings();

    // 使用正确的数据结构来存储设置
    struct GNOMESetting {
        QString schema;
        QString key;
        QString value;
    };
    
    QVector<GNOMESetting> disabledSettings = {
        // Super 键
        {"org.gnome.mutter", "overlay-key", "''"},

        // 窗口管理
        {"org.gnome.desktop.wm.keybindings", "close", "['']"},
        {"org.gnome.desktop.wm.keybindings", "minimize", "['']"},
        {"org.gnome.desktop.wm.keybindings", "maximize", "['']"},
        {"org.gnome.desktop.wm.keybindings", "unmaximize", "['']"},
        {"org.gnome.desktop.wm.keybindings", "begin-move", "['']"},
        {"org.gnome.desktop.wm.keybindings", "begin-resize", "['']"},
        {"org.gnome.desktop.wm.keybindings", "toggle-fullscreen", "['']"},
        {"org.gnome.desktop.wm.keybindings", "toggle-maximized", "['']"},
        
        // 工作区
        {"org.gnome.desktop.wm.keybindings", "switch-to-workspace-left", "['']"},
        {"org.gnome.desktop.wm.keybindings", "switch-to-workspace-right", "['']"},
        {"org.gnome.desktop.wm.keybindings", "switch-to-workspace-up", "['']"},
        {"org.gnome.desktop.wm.keybindings", "switch-to-workspace-down", "['']"},
        {"org.gnome.desktop.wm.keybindings", "move-to-workspace-left", "['']"},
        {"org.gnome.desktop.wm.keybindings", "move-to-workspace-right", "['']"},
        {"org.gnome.desktop.wm.keybindings", "move-to-workspace-up", "['']"},
        {"org.gnome.desktop.wm.keybindings", "move-to-workspace-down", "['']"},
        
        // 面板和 Shell
        {"org.gnome.shell.keybindings", "toggle-application-view", "['']"},
        {"org.gnome.shell.keybindings", "toggle-message-tray", "['']"},
        {"org.gnome.shell.keybindings", "focus-active-notification", "['']"},
        {"org.gnome.shell.keybindings", "toggle-overview", "['']"},
        // {"org.gnome.shell.keybindings", "screenshot", "['']"},
        // {"org.gnome.shell.keybindings", "screenshot-window", "['']"},
        // {"org.gnome.shell.keybindings", "show-screenshot-ui", "['']"},
        // {"org.gnome.shell.keybindings", "show-screen-recording-ui", "['']"},
        
        // 媒体键
        {"org.gnome.settings-daemon.plugins.media-keys", "home", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "email", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "www", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "calculator", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "screensaver", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "media", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "volume-up", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "volume-down", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "volume-mute", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "next", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "previous", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "play", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "pause", "['']"},
        {"org.gnome.settings-daemon.plugins.media-keys", "stop", "['']"},
        
        // 自定义快捷键
        {"org.gnome.settings-daemon.plugins.media-keys", "custom-keybindings", "['']"},
        
        // 其他系统快捷键
        {"org.gnome.desktop.wm.keybindings", "panel-main-menu", "['']"},
        {"org.gnome.desktop.wm.keybindings", "panel-run-dialog", "['']"},
        {"org.gnome.desktop.wm.keybindings", "switch-applications", "['']"},
        {"org.gnome.desktop.wm.keybindings", "switch-applications-backward", "['']"},
        {"org.gnome.desktop.wm.keybindings", "switch-windows", "['']"},
        {"org.gnome.desktop.wm.keybindings", "switch-windows-backward", "['']"},
        {"org.gnome.desktop.wm.keybindings", "switch-group", "['']"},
        {"org.gnome.desktop.wm.keybindings", "switch-group-backward", "['']"}

    };
    
    // 应用设置
    bool allSuccess = true;
    int successCount = 0;
    int totalCount = disabledSettings.size();
    
    for (const GNOMESetting &setting : disabledSettings) {
        if (runCommand("gsettings", {"set", setting.schema, setting.key, setting.value})) {
            successCount++;
            qDebug(log) << "Disabled:" << setting.schema << setting.key;
        } else {
            qCritical(log) << "Disable fail:" << setting.schema << setting.key;
            allSuccess = false;
        }
    }

    qDebug(log) << QString("GNOME is disabled: %1/%2 Success").arg(successCount).arg(totalCount);

    /*/ 重启 GNOME Shell 使设置生效
    if (allSuccess || successCount > 0) {
        qDebug(log) << "Reboot GNOME Shell ......";
        if (runCommand("killall", {"-3", "gnome-shell"})) {
            qDebug(log) << "GNOME Shell reboot signal is sended";
        } else {
            qWarning(log) << "Reboot GNOME Shell fail, Some settings may require you to log in again to take effect.";
        }
    } //*/

    return allSuccess;
}

bool CDesktopShortcutManager::restoreGNOMEShortcuts()
{
    qDebug(log) << "Restore GNOME shortcuts ......";

    if (m_gnomeSettings.isEmpty()) {
        qWarning(log) << "No backup settings are available to restore";
        return false;
    }
    
    bool allSuccess = true;
    int successCount = 0;
    int totalCount = m_gnomeSettings.size();
    
    // 恢复备份的设置
    for (auto it = m_gnomeSettings.begin(); it != m_gnomeSettings.end(); ++it) {
        QStringList parts = it.key().split("|");
        if (parts.size() == 2) {
            QString schema = parts[0];
            QString key = parts[1];
            QString value = it.value().toString();
            
            if (runCommand("gsettings", {"set", schema, key, value})) {
                successCount++;
                qDebug(log) << "Restored:" << schema << key << value;
            } else {
                qCritical(log) << "Restore failed:" << schema << key << value;
                allSuccess = false;
            }
        }
    }

    qDebug(log) << QString("GNOME shortcut restoration completed: %1/%2 successful").arg(successCount).arg(totalCount);

    /*/ 重启 GNOME Shell 使设置生效
    if (successCount > 0) {
        qDebug(log) << "Restart GNOME Shell...";
        if (runCommand("killall", {"-3", "gnome-shell"})) {
            qDebug(log) << "The GNOME Shell restart signal has been sent";
        } else {
            qWarning(log) << "Failed to restart GNOME Shell; some settings may require logging in again to take effect.";
        }
    } //*/
    
    return allSuccess;
}

bool CDesktopShortcutManager::resetGNOMEShortcuts()
{
    qDebug(log) << "Reset GNOME shortcuts ......";

    if (m_gnomeSettings.isEmpty()) {
        qWarning(log) << "No backup settings are available to reset";
        return false;
    }

    bool allSuccess = true;
    int successCount = 0;
    int totalCount = m_gnomeSettings.size();

    // 恢复备份的设置
    for (auto it = m_gnomeSettings.begin(); it != m_gnomeSettings.end(); ++it) {
        QStringList parts = it.key().split("|");
        if (parts.size() == 2) {
            QString schema = parts[0];
            QString key = parts[1];

            if (runCommand("gsettings", {"reset", schema, key})) {
                successCount++;
                qDebug(log) << "Reset:" << schema << key;
            } else {
                qCritical(log) << "Reset failed:" << schema << key;
                allSuccess = false;
            }
        }
    }

    qDebug(log) << QString("GNOME shortcut reset completed: %1/%2 successful").arg(successCount).arg(totalCount);

    /*/ 重启 GNOME Shell 使设置生效
    if (successCount > 0) {
        qDebug(log) << "Restart GNOME Shell...";
        if (runCommand("killall", {"-3", "gnome-shell"})) {
            qDebug(log) << "The GNOME Shell restart signal has been sent";
        } else {
            qWarning(log) << "Failed to restart GNOME Shell; some settings may require logging in again to take effect.";
        }
    } //*/

    return allSuccess;
}

void CDesktopShortcutManager::backupGNOMESettings()
{
    qDebug(log) << "Backup GNOME settings ......";

    m_gnomeSettings.clear();

    // 重要的 GNOME 设置 schema
    QVector<QString> schemas = {
        "org.gnome.desktop.wm.keybindings",
        "org.gnome.shell.keybindings", 
        "org.gnome.settings-daemon.plugins.media-keys",
        "org.gnome.mutter"
    };

    for (const QString &schema : schemas) {
        // 获取 schema 的所有键
        QString output = getCommandOutput("gsettings", {"list-keys", schema});
        if (output.isEmpty()) {
            qWarning(log) << "Unable to retrieve the schema key:" << schema;
            continue;
        }

        QStringList keys = output.split('\n', Qt::SkipEmptyParts);
        qDebug(log) << QString("Schema %1 has %2 keys").arg(schema).arg(keys.size());

        foreach (const QString &key, keys) {
            QString value = getCommandOutput("gsettings", {"get", schema, key}).trimmed();
            if (!value.isEmpty()) {
                // 使用 schema + key 作为唯一标识
                QString settingKey = schema + "|" + key;
                m_gnomeSettings[settingKey] = value;
                qDebug(log) << "Backup:" << settingKey << "=" << value;
            } else {
                qWarning(log) << "Backup fail:" << schema + "|" + key;
            }
        }
    }

    qDebug(log) << QString("GNOME settings backup completed, a total of %1 settings were backed up").arg(m_gnomeSettings.size());
}

// KDE 快捷键管理
bool CDesktopShortcutManager::disableKDEShortcuts()
{
    qDebug(log) << "Disable KDE shortcuts ......";

    backupKDESettings();

    // 禁用 KDE 全局快捷键
    bool success = true;

    // 禁用 KWin 快捷键
    QStringList kwinKeys = {
        "Window Close", "Window Maximize", "Window Minimize", 
        "Window Fullscreen", "Window to Desktop 1", "Window to Desktop 2",
        "Switch to Desktop 1", "Switch to Desktop 2", "Expose"
    };

    for (const QString &key : kwinKeys) {
        if (!runCommand("kwriteconfig5", {"--file", "kglobalshortcutsrc", "--key", key, "none"})) {
            success = false;
        }
    }

    // 禁用修饰键快捷键
    if (!runCommand("kwriteconfig5", {"--file", "kwinrc", "--group", "ModifierOnlyShortcuts", "--key", "Meta", ""})) {
        success = false;
    }

    /*/ 重启 KWin 使设置生效
    runCommand("kwin_x11", {"--replace"});//*/

    return success;
}

bool CDesktopShortcutManager::restoreKDEShortcuts()
{
    qDebug(log) << "Restore KDE shortcuts ......";

    // 恢复备份的配置文件
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

    if (!m_kdeSettings.isEmpty()) {
        for (auto it = m_kdeSettings.begin(); it != m_kdeSettings.end(); ++it) {
            QStringList parts = it.key().split("|");
            if (parts.size() == 3) {
                QString file = parts[0];
                QString group = parts[1];
                QString key = parts[2];
                
                if (!runCommand("kwriteconfig5", {"--file", file, "--group", group, "--key", key, it.value()})) {
                    qWarning(log) << "Failed to restore KDE setting:" << file << group << key;
                }
            }
        }
    }

    /*/ 重启 KWin
    runCommand("kwin_x11", {"--replace"});//*/

    return true;
}

void CDesktopShortcutManager::backupKDESettings()
{
    qDebug(log) << "Backup KDE settings ......";

    QString configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

    // 备份重要的 KDE 配置文件
    QStringList configFiles = {
        "kglobalshortcutsrc",
        "kwinrc",
        "khotkeysrc"
    };

    for (const QString &file : configFiles) {
        QString sourcePath = configDir + "/" + file;
        if (QFile::exists(sourcePath)) {
            backupFile(sourcePath, m_kdeBackupPath + "_" + file);
        }
    }

    // 也备份关键设置到内存
    QStringList settings = {
        "kglobalshortcutsrc|kwin|Window Close",
        "kglobalshortcutsrc|kwin|Window Maximize", 
        "kglobalshortcutsrc|kwin|Window Minimize",
        "kwinrc|ModifierOnlyShortcuts|Meta"
    };

    for (const QString &setting : settings) {
        QStringList parts = setting.split("|");
        if (parts.size() == 3) {
            QString value = getCommandOutput("kreadconfig5", {"--file", parts[0], "--group", parts[1], "--key", parts[2]});
            m_kdeSettings[setting] = value.trimmed();
        }
    }
}
#endif // #if defined(Q_OS_LINUX)

// 通用工具方法
bool CDesktopShortcutManager::runCommand(const QString &program, const QStringList &args, int timeout)
{
    QProcess process;
    process.setProgram(program);
    if(!args.isEmpty())
        process.setArguments(args);

    //qDebug(log) << "Command:" << program << args;

    process.start();

    if (!process.waitForFinished(timeout)) {
        qWarning(log) << "Command timeout:" << program << args;
        return false;
    }

    if (process.exitCode() != 0) {
        QString errorOutput = process.readAllStandardError();
        qWarning(log) << "Command failed:" << program << args 
                   << "exit code:" << process.exitCode()
                   << "failed:" << errorOutput;
        return false;
    }

    QString standardOutput = process.readAllStandardOutput();
    if (!standardOutput.isEmpty()) {
        qDebug(log) << "Command output:" << standardOutput.trimmed();
    }

    return true;
}

QString CDesktopShortcutManager::getCommandOutput(const QString &program, const QStringList &args)
{
    QProcess process;
    process.start(program, args);

    if (process.waitForFinished(5000) && process.exitCode() == 0) {
        return process.readAllStandardOutput();
    }

    return "";
}

bool CDesktopShortcutManager::backupFile(const QString &sourcePath, const QString &backupPath)
{
    return QFile::copy(sourcePath, backupPath);
}

bool CDesktopShortcutManager::restoreFile(const QString &backupPath, const QString &targetPath)
{
    if (QFile::exists(targetPath)) {
        QFile::remove(targetPath);
    }
    return QFile::copy(backupPath, targetPath);
}
