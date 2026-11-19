// Author: Kang Lin <kl222@126.com>

#include <QRegularExpression>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QSaveFile>
#include <QDataStream>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    #include <QStringConverter>
#endif
#include "DesktopManagerUnix.h"
#include "ParameterPlugin.h"

static Q_LOGGING_CATEGORY(log, "Plugin.Hook.DesktopManager.Unix")

CDesktopManagerUnix::CDesktopManagerUnix(CParameterPlugin *para, QObject *parent)
    : CDesktopManager(para, parent)
{
    qDebug(log) << Q_FUNC_INFO;

    m_desktopEnv = detectDesktopEnvironment();
    qDebug(log) << "Desktop environment:" << m_desktopEnv;

    // 设置备份路径
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                      + "/Rabbit/RabbitRemoteControl/Backup";
    QDir d(tempDir);
    if(!d.exists())
        if(!d.mkpath(tempDir))
            qCritical(log) << "Failed mkpath:" << tempDir;
    m_szBackupGnome = tempDir + "/gnome_shortcuts.bak";
    m_szBackupKDE = tempDir + "/kde_shortcuts.bak";
    m_szBackupXFCE = tempDir + "/xfce_shortcuts.bak";
    m_szConfigLXQT =  QDir::homePath() + "/.config/lxqt/globalkeyshortcuts.conf";
    m_szBackupLXQT = tempDir + "/lxqt_shortcuts.bak";
}

CDesktopManagerUnix::~CDesktopManagerUnix()
{
    qDebug(log) << Q_FUNC_INFO;
    // 确保退出时恢复快捷键
    if (m_shortcutsDisabled) {
        qWarning(log) << "Shortcuts is disabled, restoring ......";
        slotRestoreShortcuts();
    }
}

QString CDesktopManagerUnix::detectDesktopEnvironment()
{
    QString xdgDesktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP", "").toLower();
    //QString desktopSession = qEnvironmentVariable("DESKTOP_SESSION", "").toLower();
    //QString gdmSession = qEnvironmentVariable("GDMSESSION", "").toLower();

    if (xdgDesktop.contains("gnome")) {
        return "GNOME";
    } else if (xdgDesktop.contains("kde")) {
        return "KDE";
    } else if (xdgDesktop.contains("xfce")) {
        return "XFCE";
    } else if (xdgDesktop.contains("mate")) {
        return "MATE";
    } else if (xdgDesktop.contains("cinnamon")) {
        return "Cinnamon";
    } else if (xdgDesktop.contains("pantheon")) {
        return "Pantheon";
    } else if (xdgDesktop.contains("lxqt")) {
        return "LXQT";
    } else {
        return "Unknown";
    }
}

bool CDesktopManagerUnix::OnDisableShortcuts()
{
    qDebug(log) << Q_FUNC_INFO;
    if (m_shortcutsDisabled) {
        qDebug(log) << "Shortcuts is disabled";
        return false;
    }

    m_shortcutsDisabled = true;
    bool success = false;

#if defined(Q_OS_LINUX)
    if("GNOME" == m_desktopEnv || "Cinnamon" == m_desktopEnv || "MATE" == m_desktopEnv) {
        success = disableGNOMEShortcuts();
    } else if (m_desktopEnv == "KDE") {
        success = disableKDEShortcuts();
    } else if (m_desktopEnv == "XFCE") {
        success = disableXFCEShortcuts();
    } else if (m_desktopEnv == "LXQT") {
        success = disableLXQTShortcuts();
    } else {
        qWarning(log) << "Unsupported desktop environment:" << m_desktopEnv;
        return false;
    }
#endif

    if (success) {
        qInfo(log) << "All desktop shortcuts have been disabled";
    }

    return success;
}

bool CDesktopManagerUnix::OnRestoreShortcuts()
{
    qDebug(log) << Q_FUNC_INFO;
    if (!m_shortcutsDisabled) {
        qDebug(log) << "The shortcut key is not disabled, no need to restore.";
        return false;
    }

    m_shortcutsDisabled = false;
    bool success = false;

#if defined(Q_OS_LINUX)
    if (m_desktopEnv == "GNOME") {
        success = restoreGNOMEShortcuts();
        //resetGNOMEShortcuts();
    } else if (m_desktopEnv == "KDE") {
        success = restoreKDEShortcuts();
    } else if (m_desktopEnv == "XFCE") {
        success = restoreXFCEShortcuts();
    } else if (m_desktopEnv == "LXQT") {
        success = restoreLXQTShortcuts();
    } else {
        qWarning(log) << "Unsupported desktop environment:" << m_desktopEnv;
        return false;
    }
#endif

    if (success) {
        qInfo(log) << "All desktop shortcuts have been restored";
    }

    return success;
}

/*!
 * \brief 禁止 GNOME 快捷键
 * \return 如果有错误则返回 false， 全对则返回 true
 * \details 可以在系统中 "设置 -> 键盘 -> 键盘快捷键" 查看和设置快捷键
 */
bool CDesktopManagerUnix::disableGNOMEShortcuts()
{
    qDebug(log) << "Disable GNOME shortcuts ......";

    // 备份当前设置
    backupGNOMESettings();

    // 禁止设置
    bool bAllSuccess = true;
    int nSuccessCount = 0;
    int nTotalCount = 0;
    for (auto it = m_gnomeSettings.begin(); it != m_gnomeSettings.end(); ++it) {
        QString szSchema = it.key();
        auto keys = it.value();
        for(auto itKey = keys.begin(); itKey != keys.end(); itKey++) {
            nTotalCount++;
            QString szKey = itKey.key();
            // Super 键
            if("org.gnome.mutter" == szSchema && "overlay-key" == szKey) {
                if (RunCommand("gsettings", {"set", szSchema, szKey, "''"})) {
                    nSuccessCount++;
                    //qDebug(log) << "Disabled:" << szSchema + ": " + szKey + " = " + "''";
                } else {
                    qCritical(log) << "Disabled failed:" << szSchema + ": " + szKey + " = " + "''";
                    bAllSuccess = false;
                }
                continue;
            }

            QString szValue = itKey.value();
            // 是否包含 "[]"
            // 方法一：直接判断包含 []
            //if(!(szValue.contains('[') && szValue.contains(']'))) continue;
            // 方法二：使用正则表达式: \[.*\]
            if(!szValue.contains(QRegularExpression("\\[.*\\]"))) continue;

            // 去掉 [] 中的内容:
            /*
             * 方法一：正则表达式写法:
             *
             *   (?<=\[)[^\]]*(?=\])
             *
             * 说明：
             * - (?<=\[) — 正向肯定后顾，匹配在 [ 之后的位置
             * - [^\]]* — 匹配任意不是 ] 的字符（0个或多个）
             * - (?=\]) — 正向肯定前瞻，匹配在 ] 之前的位置
             *
             * 这样匹配到的就是括号内的内容，不包括括号本身。
             * 注意：QRegularExpression 对前瞻/后顾的支持
             * Qt 的 QRegularExpression（基于 PCRE）支持前瞻 (?=) 和后顾 (?<=)，所以上面的写法是有效的。
             */
            //szValue = szValue.replace(QRegularExpression("(?<=\\[)[^\\]]*(?=\\])"), "");
            /* 方法二：使用捕获组
             * 正则表达式：
             *  \[([^\]]*)\]
             * 说明:
             * |正则    |C++ 字符串|含义               |
             * |-------|---------|-------------------|
             * |\[     |"\\["    |匹配左方括号 [       |
             * |(      |"("      |开始捕获组           |
             * |[^\]]  |"[^\\]]" |匹配任意不是 ] 的字符 |
             * |*      |"*"      |重复 0 次或多次      |
             * |)      |")"      |结束捕获组           |
             * |\]     |"\\]"    |匹配右方括号 ]       |
             *
             * 注意： [^\\]] 在正则中表示字符类 [^]]（匹配非 ] 的字符），但在 C++ 字符串中需要对 \ 和 ] 进行转义。
             */
            szValue.replace(QRegularExpression("\\[([^\\]]*)\\]"), "[]");
            if (RunCommand("gsettings", {"set", szSchema, szKey, szValue})) {
                nSuccessCount++;
                //qDebug(log) << "Disabled:" << szSchema + ": " + szKey + " = " + szValue;
            } else {
                qCritical(log) << "Disabled failed:" << szSchema + ": " + szKey + " = " + szValue;
                bAllSuccess = false;
            }
        }
    }

    qDebug(log) << QString("GNOME is disabled: %1/%2 Success").arg(nSuccessCount).arg(nTotalCount);

    return bAllSuccess;
}

/*!
 * \brief 恢复 GNOME 桌面快捷键
 * \return 如果有错误则返回 false， 全对则返回 true
 */
bool CDesktopManagerUnix::restoreGNOMEShortcuts()
{
    qDebug(log) << "Restore GNOME shortcuts ......";

    if (m_gnomeSettings.isEmpty()) {
        qWarning(log) << "No backup settings are available to restore";
        return false;
    }

    bool bAllSuccess = true;
    int nSuccessCount = 0;
    int nTotalCount = 0;

    // 恢复备份的设置
    for (auto it = m_gnomeSettings.begin(); it != m_gnomeSettings.end(); ++it) {
        QString szSchema = it.key();
        auto keys = it.value();
        for(auto itKey = keys.begin(); itKey != keys.end(); itKey++) {
            nTotalCount++;
            QString szKey = itKey.key();
            QString szValue = itKey.value();
            if (RunCommand("gsettings", {"set", szSchema, szKey, szValue})) {
                nSuccessCount++;
                //qDebug(log) << "Restored:" << szSchema + ": " + szKey + " = " + szValue;
            } else {
                qCritical(log) << "Restore failed:" << szSchema + ": " + szKey + " = " + szValue;
                bAllSuccess = false;
            }
        }
    }

    qDebug(log) << QString("GNOME shortcut restoration completed: %1/%2 successful").arg(nSuccessCount).arg(nTotalCount);

    /* 重启 GNOME Shell 使设置生效。锁屏和注销需要重启才能生效
    if (successCount > 0) {
        qDebug(log) << "Restart GNOME Shell...";
        if (RunCommand("killall", {"-3", "gnome-shell"})) {
            qDebug(log) << "The GNOME Shell restart signal has been sent";
        } else {
            qWarning(log) << "Failed to restart GNOME Shell; some settings may require logging in again to take effect.";
        }
    } //*/

    QFile file(m_szBackupGnome);
    if(!file.remove())
        qCritical(log) << "Failed Remove file" << m_szBackupGnome;

    return bAllSuccess;
}

/*!
 * \brief 重置 GNOME 桌面快捷键
 * \return 如果有错误则返回 false， 全对则返回 true
 */
bool CDesktopManagerUnix::resetGNOMEShortcuts()
{
    qDebug(log) << "Reset GNOME shortcuts ......";

    if (m_gnomeSettings.isEmpty()) {
        qWarning(log) << "No backup settings are available to reset";
        return false;
    }

    bool bAllSuccess = true;
    int nSuccessCount = 0;
    int nTotalCount = 0;

    // 重置
    for (auto it = m_gnomeSettings.begin(); it != m_gnomeSettings.end(); ++it) {
        QString szSchema = it.key();
        auto keys = it.value();
        for(auto itKey = keys.begin(); itKey != keys.end(); itKey++) {
            nTotalCount++;
            QString szKey = itKey.key();
            if (RunCommand("gsettings", {"reset", szSchema, szKey})) {
                nSuccessCount++;
                //qDebug(log) << "Reset:" << szSchema + ": " + szKey;
            } else {
                qCritical(log) << "Reset failed:" << szSchema + ": " + szKey;
                bAllSuccess = false;
            }
        }
    }

    qDebug(log) << QString("GNOME shortcut reset completed: %1/%2 successful").arg(nSuccessCount).arg(nTotalCount);

    /* 重启 GNOME Shell 使设置生效。锁屏和注销需要重启才能生效
    if (successCount > 0) {
        qDebug(log) << "Restart GNOME Shell...";
        if (RunCommand("killall", {"-3", "gnome-shell"})) {
            qDebug(log) << "The GNOME Shell restart signal has been sent";
        } else {
            qWarning(log) << "Failed to restart GNOME Shell; some settings may require logging in again to take effect.";
        }
    } //*/

    QFile file(m_szBackupGnome);
    if(!file.remove())
        qCritical(log) << "Failed Remove file" << m_szBackupGnome;

    return bAllSuccess;
}

/*!
 * \brief 备份 GNOME 桌面快捷键
 * \return 返回备份的个数
 */
int CDesktopManagerUnix::backupGNOMESettings()
{
    int nTotalCount = 0;

    qDebug(log) << "Backup GNOME settings ......";
    m_gnomeSettings.clear();

    if(QFile::exists(m_szBackupGnome)) {
        qWarning(log) << "The last startup ended unexpectedly! Load from the GNOME back up file" << m_szBackupGnome;
        QFile in(m_szBackupGnome);
        if (!in.open(QIODevice::ReadOnly)) {
            qCritical(log) << "Failed: Open file" << m_szBackupGnome;
            return 0;
        }

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(in.readAll(), &err);
        in.close();

        if (err.error != QJsonParseError::NoError || !doc.isObject()) {
            qCritical(log) << "Invalid JSON:" << err.errorString();
            return 0;
        }

        const QJsonObject root = doc.object();
        for (auto itSchema = root.constBegin(); itSchema != root.constEnd(); ++itSchema) {
            QMap<QString, QString> keys;
            const QJsonObject keysObj = itSchema.value().toObject();
            for (auto it = keysObj.constBegin(); it != keysObj.constEnd(); ++it) {
                keys.insert(it.key(), it.value().toString());
                nTotalCount++;
            }
            m_gnomeSettings.insert(itSchema.key(), keys);
        }
        qDebug(log) << QString("GNOME settings backup completed, load a total of %1 settings from back up file").arg(nTotalCount);
    } else {

        struct GNOMESetting {
            QString schema;
            QString key;
        };
        QVector<GNOMESetting> vKeys = {
            // Super 键
            {"org.gnome.mutter", "overlay-key"}
        };
        foreach (auto setting, vKeys) {
            QMap<QString, QString> mKey;
            QString value = GetCommandOutput("gsettings", {"get", setting.schema, setting.key}).trimmed();
            if (!value.isEmpty()) {
                nTotalCount++;
                mKey.insert(setting.key, value);
                //qDebug(log) << "Backup:" << setting.schema + ": " + setting.key << "=" << value;
            } else {
                qWarning(log) << "Backup fail:" << setting.schema + ": " + setting.key;
            }
            if(!mKey.isEmpty())
                m_gnomeSettings.insert(setting.schema, mKey);
        }

        QVector<QString> vSchemas = {
            // 媒体键
            "org.gnome.settings-daemon.plugins.media-keys"
        };

        // 可以使用下面命令得到 schema
        // `gsettings list-schemas|grep keybindings`
        QString szSchemas = GetCommandOutput("gsettings", {"list-schemas"});
        //qDebug(log) << "Schemas:\n" << szSchemas;
        QStringList lstSchemas = szSchemas.split('\n', Qt::SkipEmptyParts);
        foreach (const QString &schema, lstSchemas) {
            if(!(schema.contains("keybindings") || schema.contains("media-keys")))
                continue;
            vSchemas.append(schema);
        }

        foreach (const QString &schema, vSchemas) {
            // 获取 schema 的所有键
            QString output = GetCommandOutput("gsettings", {"list-keys", schema});
            if (output.isEmpty()) {
                qWarning(log) << "Unable to retrieve the schema key:" << schema;
                continue;
            }

            QMap<QString, QString> vKey;
            QStringList keys = output.split('\n', Qt::SkipEmptyParts);
            //qDebug(log) << QString("Schema %1 has %2 keys").arg(schema).arg(keys.size());
            foreach (const QString &key, keys) {
                QString value = GetCommandOutput("gsettings", {"get", schema, key}).trimmed();
                if (!value.isEmpty()) {
                    nTotalCount++;
                    vKey.insert(key, value);
                    //qDebug(log) << "Backup:" << schema + ": " + key << "=" << value;
                } else {
                    qWarning(log) << "Backup fail:" << schema + ": " + key;
                }
            }
            if(!vKey.isEmpty())
                m_gnomeSettings.insert(schema, vKey);
        }

        qDebug(log) << QString("GNOME settings backup completed, a total of %1 settings were backed up").arg(nTotalCount);

        if (!m_gnomeSettings.isEmpty()) {
            QJsonObject root;
            for (auto itSchema = m_gnomeSettings.constBegin();
                 itSchema != m_gnomeSettings.constEnd(); ++itSchema) {
                QJsonObject keysObj;
                const QMap<QString, QString> &keys = itSchema.value();
                for (auto it = keys.constBegin(); it != keys.constEnd(); ++it) {
                    keysObj.insert(it.key(), it.value());
                }
                root.insert(itSchema.key(), keysObj);
            }

            QFile out(m_szBackupGnome);
            if (out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                QJsonDocument doc(root);
                out.write(doc.toJson(QJsonDocument::Indented));   // Indented 便于人阅读；Compact 更省空间
                out.close();

                if (out.error() != QFile::NoError)
                    qCritical(log) << "Failed: Write error:" << out.errorString() << "File:" << m_szBackupGnome;
            } else {
                qCritical(log) << "Failed: Open file" << m_szBackupGnome;
            }
        }
    }

    return nTotalCount;
}

/*!
 * \brief 禁止 KDE 快捷键
 * \return 如果有错误则返回 false， 全对则返回 true
 */
bool CDesktopManagerUnix::disableKDEShortcuts()
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
        if (!RunCommand("kwriteconfig5", {"--file", "kglobalshortcutsrc", "--key", key, "none"})) {
            success = false;
        }
    }

    // 禁用修饰键快捷键
    if (!RunCommand("kwriteconfig5", {"--file", "kwinrc", "--group", "ModifierOnlyShortcuts", "--key", "Meta", ""})) {
        success = false;
    }

    /*/ 重启 KWin 使设置生效
    RunCommand("kwin_x11", {"--replace"});//*/

    return success;
}

/*!
 * \brief 恢复 KDE 桌面快捷键
 * \return 如果有错误则返回 false， 全对则返回 true
 */
bool CDesktopManagerUnix::restoreKDEShortcuts()
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

                if (!RunCommand("kwriteconfig5", {"--file", file, "--group", group, "--key", key, it.value()})) {
                    qWarning(log) << "Failed to restore KDE setting:" << file << group << key;
                }
            }
        }
    }

    /*/ 重启 KWin
    RunCommand("kwin_x11", {"--replace"});//*/

    return true;
}

/*!
 * \brief 备份 KDE 桌面快捷键
 * \return 备份个数
 * \details 可以在系统中 "设置 -> 键盘 -> 键盘快捷键" 查看和设置快捷键
 *
 * KDE 快捷键存储在两个主要位置：
 * 1. ~/.config/kglobalshortcutsrc - 全局快捷键配置
 * 2. ~/.config/kwinrc - KWin 窗口管理器快捷键
 *
 * 使用 kwriteconfig5/kreadconfig5 工具进行读写
 */
int CDesktopManagerUnix::backupKDESettings()
{
    int nCount = 0;
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
            backupFile(sourcePath, m_szBackupKDE + "_" + file);
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
            QString value = GetCommandOutput("kreadconfig5", {"--file", parts[0], "--group", parts[1], "--key", parts[2]});
            m_kdeSettings[setting] = value.trimmed();
        }
    }

    return nCount;
}

/*!
 * \brief 禁止 XFCE 桌面快捷键
 * \return 如果有错误则返回 false， 全对则返回 true
 */
bool CDesktopManagerUnix::disableXFCEShortcuts()
{
    qDebug(log) << "Disable XFCE settings ......";

    if(QFile::exists(m_szBackupXFCE)) {
        qWarning(log) << "The last startup ended unexpectedly! The backup XFCE file is exits" << m_szBackupXFCE;
        loadXFCEShortcuts();
    } else {
        // 先备份当前设置
        backupXFCEShortcuts();
    }
    if (m_xfceSettings.isEmpty()) {
        qWarning(log) << "No XFCE settings to disable";
        return false;
    }

    bool bAllSuccess = true;
    int nSuccessCount = 0;
    int nTotalCount = 0;

    // 遍历所有备份的快捷键，将其设置为空或无效值
    for (auto it = m_xfceSettings.begin(); it != m_xfceSettings.end(); ++it) {
        nTotalCount++;
        QString property = it.key();
        QString value = it.value();

        /*
        // 跳过一些系统必需的快捷键（如果不想禁用的话）
        // 例如：保留窗口管理器的基本功能
        if (property.contains("/xfwm4/system/") ||
            property.contains("/xfwm4/default/")) {
            qDebug(log) << "Skip system shortcut:" << property;
            continue;
        }//*/

        // 对于 XFCE，使用 xfconf-query 重置快捷键
        // 方法1：将值设置为空字符串（对于某些类型的快捷键有效）
        // 方法2：使用 --reset 重置为默认值
        // 方法3：设置为无效的值

        QStringList args;
        args << "--channel" << "xfce4-keyboard-shortcuts";

        // 检查属性类型，有些是 string 类型，有些是 int 类型
        // 尝试重置该属性
        args << "--reset" << "--property" << property;

        if (RunCommand("xfconf-query", args)) {
            nSuccessCount++;
            //qDebug(log) << "Disabled:" << property << "=" << value;
        } else {
            // 如果重置失败，尝试设置为空值
            QStringList setArgs;
            setArgs << "--channel" << "xfce4-keyboard-shortcuts"
                    << "--property" << property
                    << "--set" << "";

            if (RunCommand("xfconf-query", setArgs)) {
                nSuccessCount++;
                qDebug(log) << "Disabled (set empty):" << property << "=" << value;
            } else {
                qCritical(log) << "Disabled failed:" << property;
                bAllSuccess = false;
            }
        }
    }

    qDebug(log) << QString("XFCE shortcuts disabled: %1/%2 success").arg(nSuccessCount).arg(nTotalCount);

    // 通知 XFCE 设置已更改
    //RunCommand("xfce4-settings-helper", {"--restart"});
    return bAllSuccess;
}

/*!
 * \brief 恢复 XFCE 桌面快捷键
 * \return 如果有错误则返回 false， 全对则返回 true
 */
bool CDesktopManagerUnix::restoreXFCEShortcuts()
{
    qDebug(log) << "Restore XFCE settings ......";

    if(QFile::exists(m_szBackupXFCE)) {
        QFile::remove(m_szBackupXFCE);
    }

    if (m_xfceSettings.isEmpty()) {
        qWarning(log) << "No backup settings are available to restore";
        return false;
    }

    bool bAllSuccess = true;
    int nSuccessCount = 0;
    int nTotalCount = 0;

    // 恢复备份的设置
    for (auto it = m_xfceSettings.begin(); it != m_xfceSettings.end(); ++it) {
        nTotalCount++;
        QString property = it.key();
        QString value = it.value();

        /*
        // 跳过系统快捷键
        if (property.contains("/xfwm4/system/") ||
            property.contains("/xfwm4/default/")) {
            qDebug(log) << "Skip system shortcut restore:" << property;
            continue;
        }//*/

        // 恢复快捷键设置
        QStringList args;
        args << "--channel" << "xfce4-keyboard-shortcuts"
             << "--property" << property
             << "--set" << value
             << "--create" << "--type" << "string";

        if (RunCommand("xfconf-query", args)) {
            nSuccessCount++;
            //qDebug(log) << "Restored:" << property << "=" << value << args;
        } else {
            qCritical(log) << "Restore failed:" << property << "=" << value;
            bAllSuccess = false;
        }
    }

    qDebug(log) << QString("XFCE shortcuts restored: %1/%2 success").arg(nSuccessCount).arg(nTotalCount);

    // 通知 XFCE 设置已更改
    //RunCommand("xfce4-settings-helper", {"--restart"});
    return bAllSuccess;
}

/*!
 * \brief 备份 XFCE 桌面快捷键
 * \return 返回备份成功的个数
 * \details 可使用命令 `xfce4-settings-manager` 打开设置对话框，
 *       或者在系统中 "设置 -> 设置管理器 -> 键盘 -> 应用程序快捷键" 查看和设置快捷键
 * - 步骤 1：查看现有快捷键
 *   运行以下命令列出所有已配置的快捷键：
 *   `xfconf-query --channel xfce4-keyboard-shortcuts -lv`
 * - 步骤 2：添加新快捷键
 *   例如，将 Ctrl+Alt+T 设置为打开终端：
 *
 *   ```
 *   xfconf-query --create --channel xfce4-keyboard-shortcuts \
 *   --property "/commands/custom/<Primary><Alt>T" \
 *   --type string --set "xfce4-terminal"
 *   ```
 *
 * - 步骤 3：删除快捷键
 *   取消某个快捷键（如 Ctrl+Alt+Left）：
 *
 *   ```
 *   xfconf-query --reset --channel xfce4-keyboard-shortcuts \
 *   --property "/xfwm4/custom/<Primary><Alt>Left"
 *   ```
 *
 * 注意事项
 * <Primary> 指的是 Ctrl, <Super> 指的是 Windows 键.
 * 快捷键配置文件位于 ~/.config/xfce4/xfconf/xfce-perchannel-xml/xfce4-keyboard-shortcuts.xml，可以手动编辑以实现更高级的自定义
 * Both methods allow you to efficiently set up shortcuts tailored to your workflow!
 */
int CDesktopManagerUnix::backupXFCEShortcuts()
{
    int nCount = 0;
    qDebug(log) << "Backup XFCE settings ......";

    // 清空之前备份
    m_xfceSettings.clear();

    // 获取 XFCE 键盘快捷键的所有配置
    // xfconf-query --channel xfce4-keyboard-shortcuts -lv
    QString output = GetCommandOutput("xfconf-query", {"--channel", "xfce4-keyboard-shortcuts", "-lv"});
    if (output.isEmpty()) {
        qWarning(log) << "Unable to get XFCE keyboard shortcuts configuration";
        return 0;
    }

    // 解析输出格式: "/property/path value"
    // 例如: "/xfwm4/custom/<Alt>F4 close_window"
    //       "/commands/custom/<Primary><Alt>T xfce4-terminal"
    //       "/xfwm4/custom/<Super>d show_desktop"
    // 注意：有些属性可能只有路径而没有值，需要跳过
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    foreach(const QString &line, lines) {
        // 使用正则表达式解析: 路径 + 值
        // 格式: "/path/to/property value"
        QRegularExpression regex("^(\"[^\"]*\"|/[^\\s]+)\\s+(.*)$");
        QRegularExpressionMatch match = regex.match(line);

        if (match.hasMatch()) {
            QString property = match.captured(1);
            // 移除可能的引号
            if (property.startsWith('"') && property.endsWith('"')) {
                property = property.mid(1, property.length() - 2);
            }

            QString value = match.captured(2).trimmed();

            // 存储配置到内存
            m_xfceSettings[property] = value;
            nCount++;
            //qDebug(log) << "Backup:" << property << " = " << value;
        } else {
            // 可能是一些只有属性没有值的行，或者是其他格式
            qDebug(log) << "Skipping line (unable to parse):" << line;
        }
    }

    qDebug(log) << QString("XFCE settings backup completed, a total of %1 settings were backed up").arg(nCount);

    QFile out(m_szBackupXFCE);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCritical(log) << "Failed: open file:" << m_szBackupXFCE;
        return nCount;
    }
    QTextStream ts(&out);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    ts.setEncoding(QStringConverter::Utf8);
#else
    ts.setCodec("UTF-8");
#endif
    ts << output;
    out.close();

    /*
    // 同时备份配置文件本身作为额外保障
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
                         + "/xfce4/xfconf/xfce-perchannel-xml/xfce4-keyboard-shortcuts.xml";

    if (QFile::exists(configPath)) {
        // 构建备份路径
        QString backupDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        QString backupFile = backupDir + "/xfce4-keyboard-shortcuts_backup.xml";

        if (backupFile(configPath, backupFile)) {
            qDebug(log) << "Backup configuration file:" << configPath << "->" << backupFile;
        } else {
            qWarning(log) << "Failed to backup configuration file:" << configPath;
        }
    }//*/

    return nCount;
}

int CDesktopManagerUnix::loadXFCEShortcuts()
{
    if(!QFile::exists(m_szBackupXFCE)) {
        qCritical(log) << "The XFCE backup file is not exist" << m_szBackupXFCE;
        return -1;
    }

    int nCount = 0;
    qDebug(log) << "Load XFCE settings from backup file ......";

    QString output;
    QFile in(m_szBackupXFCE);
    if (!in.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical(log) << "Failed: open XFCE back file:" << m_szBackupXFCE;
        return nCount;
    }
    output = in.readAll();
    in.close();

    if(output.isEmpty()) {
        qCritical(log) << "The XFCE backup file is empty" << m_szBackupXFCE;
        return 0;
    }

    // 清空之前备份
    m_xfceSettings.clear();
    // 解析输出格式: "/property/path value"
    // 例如: "/xfwm4/custom/<Alt>F4 close_window"
    //       "/commands/custom/<Primary><Alt>T xfce4-terminal"
    //       "/xfwm4/custom/<Super>d show_desktop"
    // 注意：有些属性可能只有路径而没有值，需要跳过
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    foreach(const QString &line, lines) {
        // 使用正则表达式解析: 路径 + 值
        // 格式: "/path/to/property value"
        QRegularExpression regex("^(\"[^\"]*\"|/[^\\s]+)\\s+(.*)$");
        QRegularExpressionMatch match = regex.match(line);

        if (match.hasMatch()) {
            QString property = match.captured(1);
            // 移除可能的引号
            if (property.startsWith('"') && property.endsWith('"')) {
                property = property.mid(1, property.length() - 2);
            }

            QString value = match.captured(2).trimmed();

            // 存储配置到内存
            m_xfceSettings[property] = value;
            nCount++;
            //qDebug(log) << "Load:" << property << " = " << value;
        } else {
            // 可能是一些只有属性没有值的行，或者是其他格式
            qDebug(log) << "Skipping line (unable to parse):" << line;
        }
    }

    qDebug(log) << QString("XFCE settings is load from backup file, a total of %1 settings were load").arg(nCount);

    return nCount;
}

bool CDesktopManagerUnix::disableLXQTShortcuts()
{
    qDebug(log) << "Disable LXQT settings ......";
    if(QFile::exists(m_szConfigLXQT)) {
        if(QFile::exists(m_szBackupLXQT))
            qWarning(log) << "The last startup ended unexpectedly !!!";
        else {
            if(!QFile::copy(m_szConfigLXQT, m_szBackupLXQT))
                qCritical(log) << "Failed to backup" << m_szConfigLXQT << "to" << m_szBackupLXQT;
        }
        QRegularExpression enabled("^Enabled=.*$", QRegularExpression::MultilineOption);
        if(!replaceInFile(m_szConfigLXQT, enabled, "Enabled=false"))
            qCritical(log) << "Failed to replace Enabled in file:" << m_szConfigLXQT;
        QRegularExpression reAllowGrabBaseKeypad("^AllowGrabBaseKeypad=.*$", QRegularExpression::MultilineOption);
        if(!replaceInFile(m_szConfigLXQT, reAllowGrabBaseKeypad, "AllowGrabBaseKeypad=false"))
            qCritical(log) << "Failed to replace AllowGrabBaseKeypad in file:" << m_szConfigLXQT;
        QRegularExpression reAllowGrabBaseSpecial("^AllowGrabBaseSpecial=.*$", QRegularExpression::MultilineOption);
        if(!replaceInFile(m_szConfigLXQT, reAllowGrabBaseSpecial, "AllowGrabBaseSpecial=false"))
            qCritical(log) << "Failed to replace AllowGrabBaseSpecial in file:" << m_szConfigLXQT;
        QRegularExpression reAllowGrabLocks("^AllowGrabLocks=.*$", QRegularExpression::MultilineOption);
        if(!replaceInFile(m_szConfigLXQT, reAllowGrabLocks, "AllowGrabLocks=false"))
            qCritical(log) << "Failed to replace AllowGrabLocks in file:" << m_szConfigLXQT;
        QRegularExpression reAllowGrabMiscKeypad("^AllowGrabMiscKeypad=.*$", QRegularExpression::MultilineOption);
        if(!replaceInFile(m_szConfigLXQT, reAllowGrabMiscKeypad, "AllowGrabMiscKeypad=false"))
            qCritical(log) << "Failed to replace AllowGrabMiscKeypad in file:" << m_szConfigLXQT;
        QRegularExpression reAllowGrabMiscSpecial("^AllowGrabMiscSpecial=.*$", QRegularExpression::MultilineOption);
        if(!replaceInFile(m_szConfigLXQT, reAllowGrabMiscSpecial, "AllowGrabMiscSpecial=false"))
            qCritical(log) << "Failed to replace Enabled in file:" << m_szConfigLXQT;
        QRegularExpression reMultipleActionsBehaviour("^MultipleActionsBehaviour=.*$", QRegularExpression::MultilineOption);
        if(!replaceInFile(m_szConfigLXQT, reMultipleActionsBehaviour, "MultipleActionsBehaviour=false"))
            qCritical(log) << "Failed to replace MultipleActionsBehaviour in file:" << m_szConfigLXQT;
        restartLXQTShortcuts();
    }
    return disableXFCEShortcuts();
}

bool CDesktopManagerUnix::restoreLXQTShortcuts()
{
    if(QFile::exists(m_szBackupLXQT)) {
        if(QFile::exists(m_szConfigLXQT))
            if(!QFile::remove(m_szConfigLXQT))
                qCritical(log) << "Failed remove file:" << m_szConfigLXQT;
        if(!QFile::rename(m_szBackupLXQT, m_szConfigLXQT))
            qCritical(log) << "Failed rename file:" << m_szBackupLXQT << "to" << m_szConfigLXQT;
        restartLXQTShortcuts();
    }
    return restoreXFCEShortcuts();
}

void CDesktopManagerUnix::restartLXQTShortcuts()
{
    qDebug(log) << "restartLXQTShortcuts";
    RunCommand("pkill", QStringList() << "-f" << "lxqt-globalkeysd");
    if(!QProcess::startDetached("lxqt-globalkeysd", QStringList() << "&"))
        qCritical(log) << "Failed: Start lxqt-globalkeysd";
    return;
    auto *process = new QProcess(this);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [process](int, QProcess::ExitStatus) {
                qDebug(log) << "pkill finished";
                QProcess::startDetached("lxqt-globalkeysd");
                process->deleteLater();
            });
    process->start("pkill", QStringList() << "-f" << "lxqt-globalkeysd");
}

bool CDesktopManagerUnix::backupFile(const QString &sourcePath, const QString &backupPath)
{
    return QFile::copy(sourcePath, backupPath);
}

bool CDesktopManagerUnix::restoreFile(const QString &backupPath, const QString &targetPath)
{
    if (QFile::exists(targetPath)) {
        QFile::remove(targetPath);
    }
    return QFile::copy(backupPath, targetPath);
}

bool CDesktopManagerUnix::replaceInFile(
    const QString &file, const QRegularExpression &from, const QString &to)
{
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.setEncoding(QStringConverter::Utf8);
#else
    in.setCodec("UTF-8");
#endif
    QString content = in.readAll();
    f.close();

    if (!from.match(content).hasMatch())
        return true;

    content.replace(from, to);

    // 写临时文件后原子替换，避免中途失败损坏原文件
    QSaveFile out(file);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream ts(&out);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    ts.setEncoding(QStringConverter::Utf8);
#else
    ts.setCodec("UTF-8");
#endif
    ts << content;
    ts.flush();
    if (ts.status() != QTextStream::Ok)
        return false;

    return out.commit();
}
