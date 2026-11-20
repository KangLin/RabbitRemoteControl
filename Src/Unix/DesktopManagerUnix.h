// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QThread>
#include <QString>
#include <QProcess>
#include <QMap>
#include <QSettings>
#include <QFile>
#include <QDir>
#include "DesktopManager.h"

/*!
 * \brief Manage desktop shortcut keys
 */
class CDesktopManagerUnix : public CDesktopManager
{
    Q_OBJECT

public:
    explicit CDesktopManagerUnix(CParameterPlugin* para, QObject *parent = nullptr);
    virtual ~CDesktopManagerUnix();

    // 状态查询
    bool isDisabled() const { return m_shortcutsDisabled; }
    QString desktopEnvironment() const { return m_desktopEnv; }

    // 检测方法
    static QString detectDesktopEnvironment();

private:
    bool OnDisableShortcuts();
    bool OnRestoreShortcuts();

private:
    QString m_desktopEnv;
    bool m_shortcutsDisabled = false;

    // 存储原始设置
    QMap<QString, QMap<QString, QString> > m_gnomeSettings;
    QMap<QString, QString> m_kdeSettings;
    QMap<QString, QString> m_xfceSettings;  // XFCE 快捷键配置备份
    QString m_szBackupGnome;
    QString m_szBackupKDE;
    QString m_szBackupXFCE;
    QString m_szConfigLXQT;
    QString m_szBackupLXQT;

    // GNOME 方法
    bool disableGNOMEShortcuts();
    bool restoreGNOMEShortcuts();
    bool resetGNOMEShortcuts();
    int backupGNOMESettings();
    
    // KDE 方法
    bool disableKDEShortcuts();
    bool restoreKDEShortcuts();
    int backupKDESettings();

    // XFCE 方法
    bool disableXFCEShortcuts();
    bool restoreXFCEShortcuts();
    int backupXFCEShortcuts();
    int loadXFCEShortcuts();

    // LXQT 方法
    bool disableLXQTShortcuts();
    bool restoreLXQTShortcuts();
    void restartLXQTShortcuts();

    // 通用方法
    bool backupFile(const QString &sourcePath, const QString &backupPath);
    bool restoreFile(const QString &backupPath, const QString &targetPath);

    bool replaceInFile(const QString &file, const QRegularExpression &from, const QString &to);
};
