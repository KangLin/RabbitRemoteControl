#include <QLoggingCategory>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QSettings>

#ifdef Q_OS_WIN
#include <windows.h>
#include <shlwapi.h>
#endif

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#endif

#include "SystemProtocolHandler.h"
#include "ProtocolManager.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.ProtocolManager")
CProtocolManager::CProtocolManager(QObject *parent) : QObject(parent)
{
}

QStringList CProtocolManager::getAllRegisteredProtocols()
{
#ifdef Q_OS_LINUX
    return getRegisteredProtocolsLinux();
#elif defined(Q_OS_WIN)
    return getRegisteredProtocolsWindows();
#elif defined(Q_OS_MAC)
    return getRegisteredProtocolsMacOS();
#else
    return QStringList();
#endif
}

// ==================== Linux 实现 ====================
QStringList CProtocolManager::getRegisteredProtocolsLinux()
{
    QStringList protocols;

    // 1. 扫描所有 .desktop 文件获取协议
    scanDesktopFilesForProtocols(protocols);

    // 2. 从 mimeapps.list 读取
    QStringList configPaths = {
        QDir::homePath() + "/.config/mimeapps.list",
        QDir::homePath() + "/.local/share/applications/mimeapps.list",
        "/etc/xdg/mimeapps.list",
        "/usr/share/applications/mimeapps.list"
    };

    for (const QString &configPath : configPaths) {
        QFile configFile(configPath);
        if (configFile.exists() && configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&configFile);
            QString line;
            bool inAddedAssociations = false;
            bool inDefaultApplications = false;

            while (stream.readLineInto(&line)) {
                if (line.startsWith("[") && line.endsWith("]")) {
                    inAddedAssociations = (line == "[Added Associations]");
                    inDefaultApplications = (line == "[Default Applications]");
                    continue;
                }

                if ((inAddedAssociations || inDefaultApplications) && line.contains("=")) {
                    QStringList parts = line.split('=', Qt::SkipEmptyParts);
                    if (parts.size() >= 2) {
                        QString key = parts[0].trimmed();
                        if (key.startsWith("x-scheme-handler/")) {
                            QString protocol = key.mid(17);
                            if (!protocols.contains(protocol)) {
                                protocols.append(protocol);
                            }
                        }
                    }
                }
            }
            configFile.close();
        }
    }

    /*
    // 3. 使用 xdg-mime 查询常见协议（修正版）
    QStringList commonProtocols = {
        "http", "https", "ftp", "mailto", "tel", "sms", "callto",
        "skype", "slack", "spotify", "zoommtg", "msteams", "discord",
        "git", "ssh", "vnc", "rdp", "rrc"
    };

    for (const QString &protocol : commonProtocols) {
        QProcess process;
        process.start("xdg-mime", QStringList() << "query" << "default"
                                                << "x-scheme-handler/" + protocol);
        if (process.waitForFinished(1000)) {
            QString output = process.readAllStandardOutput().trimmed();
            if (!output.isEmpty() && !protocols.contains(protocol)) {
                protocols.append(protocol);
            }
        }
    }//*/

    // 4. 从 mimeinfo.cache 读取（系统级缓存）
    QStringList cachePaths = {
        "/usr/share/applications/mimeinfo.cache",
        "/usr/local/share/applications/mimeinfo.cache",
        QDir::homePath() + "/.local/share/applications/mimeinfo.cache"
    };

    for (const QString &cachePath : cachePaths) {
        QFile cacheFile(cachePath);
        if (cacheFile.exists() && cacheFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&cacheFile);
            QString line;
            while (stream.readLineInto(&line)) {
                if (line.startsWith("x-scheme-handler/")) {
                    QStringList parts = line.split('=', Qt::SkipEmptyParts);
                    if (parts.size() >= 2) {
                        QString key = parts[0].trimmed();
                        QString protocol = key.mid(17);
                        if (!protocols.contains(protocol)) {
                            protocols.append(protocol);
                        }
                    }
                }
            }
            cacheFile.close();
        }
    }

    qDebug() << "Linux: Found" << protocols.size() << "registered protocols";
    return protocols;
}

// ==================== 扫描 .desktop 文件 ====================
void CProtocolManager::scanDesktopFilesForProtocols(QStringList &protocols)
{
    QStringList searchPaths = {
        QDir::homePath() + "/.local/share/applications",
        "/usr/share/applications",
        "/usr/local/share/applications",
        "/var/lib/flatpak/exports/share/applications",
        QDir::homePath() + "/.local/share/flatpak/exports/share/applications"
    };

    for (const QString &path : searchPaths) {
        QDir dir(path);
        if (dir.exists()) {
            QStringList desktopFiles = dir.entryList(QStringList() << "*.desktop");
            for (const QString &file : desktopFiles) {
                parseDesktopFile(dir.absolutePath() + QDir::separator() + file, protocols);
            }
        }
    }
}

void CProtocolManager::parseDesktopFile(const QString &filePath, QStringList &protocols)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    QString line;
    bool inDesktopEntry = false;

    while (stream.readLineInto(&line)) {
        if (line.startsWith("[Desktop Entry]")) {
            inDesktopEntry = true;
            continue;
        }

        if (line.startsWith("[") && line.endsWith("]")) {
            inDesktopEntry = false;
            continue;
        }

        if (inDesktopEntry && line.startsWith("MimeType=")) {
            QString mimeTypes = line.mid(9);
            QStringList types = mimeTypes.split(';', Qt::SkipEmptyParts);
            for (const QString &type : types) {
                if (type.startsWith("x-scheme-handler/")) {
                    QString protocol = type.mid(17);
                    if (!protocols.contains(protocol)) {
                        protocols.append(protocol);
                    }
                }
            }
            break;
        }
    }
    file.close();
}

// ==================== Windows 实现 ====================
QStringList CProtocolManager::getRegisteredProtocolsWindows()
{
    QStringList protocols;

#ifdef Q_OS_WIN
    HKEY hKey;
    LONG result = RegOpenKeyExW(
        HKEY_CLASSES_ROOT,
        NULL,
        0,
        KEY_READ | KEY_ENUMERATE_SUB_KEYS,
        &hKey
        );

    if (result == ERROR_SUCCESS) {
        DWORD index = 0;
        WCHAR subKeyName[256];
        DWORD nameSize = 256;

        while (RegEnumKeyExW(hKey, index++, subKeyName, &nameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS) {
            QString keyName = QString::fromWCharArray(subKeyName);

            // 检查是否包含 URL Protocol
            HKEY subKey;
            if (RegOpenKeyExW(hKey, subKeyName, 0, KEY_READ, &subKey) == ERROR_SUCCESS) {
                DWORD type;
                if (RegQueryValueExW(subKey, L"URL Protocol", NULL, &type, NULL, NULL) == ERROR_SUCCESS) {
                    if (!keyName.startsWith(".") && !keyName.isEmpty()) {
                        protocols.append(keyName);
                    }
                }
                RegCloseKey(subKey);
            }
            nameSize = 256;
        }

        RegCloseKey(hKey);
    }

    qDebug() << "Windows: Found" << protocols.size() << "registered protocols";
#endif

    return protocols;
}

// ==================== macOS 实现 ====================
QStringList CProtocolManager::getRegisteredProtocolsMacOS()
{
    QStringList protocols;

#ifdef Q_OS_MAC
    // 使用 lsregister 命令
    QProcess process;
    process.start("/System/Library/Frameworks/CoreServices.framework/Versions/A/Frameworks/LaunchServices.framework/Versions/A/Support/lsregister",
                  QStringList() << "-dump");

    if (process.waitForFinished(3000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');

        for (const QString &line : lines) {
            if (line.contains("scheme") && line.contains("\"")) {
                // 解析 scheme 声明
                int start = line.indexOf('"');
                int end = line.indexOf('"', start + 1);
                if (start != -1 && end != -1) {
                    QString protocol = line.mid(start + 1, end - start - 1);
                    if (!protocol.isEmpty() && !protocols.contains(protocol)) {
                        protocols.append(protocol);
                    }
                }
            }
        }
    }

    // 备选方案：使用 SwiftDefaultApps (如果安装)
    QProcess swda;
    swda.start("swda", QStringList() << "getSchemes");
    if (swda.waitForFinished(2000)) {
        QString output = swda.readAllStandardOutput();
        QStringList lines = output.split('\n', Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            QString protocol = line.trimmed();
            if (!protocol.isEmpty() && !protocols.contains(protocol)) {
                protocols.append(protocol);
            }
        }
    }

    qDebug() << "macOS: Found" << protocols.size() << "registered protocols";
#endif

    return protocols;
}

// ==================== 获取默认处理程序 ====================
QString CProtocolManager::getDefaultHandlerForProtocol(const QString &protocol)
{
#ifdef Q_OS_LINUX
    return getDefaultHandlerLinux(protocol);
#elif defined(Q_OS_WIN)
    return getDefaultHandlerWindows(protocol);
#elif defined(Q_OS_MAC)
    return getDefaultHandlerMacOS(protocol);
#else
    return QString();
#endif
}

QString CProtocolManager::getDefaultHandlerLinux(const QString &protocol)
{
    // 方法1: 使用 xdg-mime
    QProcess process;
    process.start("xdg-mime", QStringList() << "query" << "default" << "x-scheme-handler/" + protocol);
    if (process.waitForFinished(1000)) {
        QString output = process.readAllStandardOutput().trimmed();
        if (!output.isEmpty()) {
            return output;
        }
    }

    // 方法2: 解析 mimeapps.list
    QStringList configPaths = {
        QDir::homePath() + "/.config/mimeapps.list",
        "/etc/xdg/mimeapps.list"
    };

    for (const QString &path : configPaths) {
        QFile file(path);
        if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            QString line;
            bool inDefaultApps = false;

            while (stream.readLineInto(&line)) {
                if (line.startsWith("[Default Applications]")) {
                    inDefaultApps = true;
                    continue;
                }
                if (line.startsWith("[") && line.endsWith("]")) {
                    inDefaultApps = false;
                    continue;
                }

                if (inDefaultApps && line.startsWith("x-scheme-handler/" + protocol + "=")) {
                    file.close();
                    return line.section('=', 1).trimmed();
                }
            }
            file.close();
        }
    }

    return QString();
}

QString CProtocolManager::getDefaultHandlerWindows(const QString &protocol)
{
#ifdef Q_OS_WIN
    QString handler;
    HKEY hKey;
    QString keyPath = QString("Software\\Classes\\%1\\shell\\open\\command").arg(protocol);

    if (RegOpenKeyExW(HKEY_CURRENT_USER, (LPCWSTR)keyPath.utf16(), 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        WCHAR buffer[1024];
        DWORD size = sizeof(buffer);
        DWORD type;

        if (RegQueryValueExW(hKey, NULL, NULL, &type, (LPBYTE)buffer, &size) == ERROR_SUCCESS) {
            handler = QString::fromWCharArray(buffer);
        }
        RegCloseKey(hKey);
    }

    return handler;
#else
    return QString();
#endif
}

QString CProtocolManager::getDefaultHandlerMacOS(const QString &protocol)
{
#ifdef Q_OS_MAC
    QProcess process;
    process.start("swda", QStringList() << "getHandler" << "--URL" << protocol);
    if (process.waitForFinished(1000)) {
        QString output = process.readAllStandardOutput().trimmed();
        if (!output.isEmpty()) {
            return output;
        }
    }
#endif
    return QString();
}

// ==================== 其他辅助方法 ====================
bool CProtocolManager::isProtocolRegistered(const QString &protocol)
{
    QStringList protocols = getAllRegisteredProtocols();
    return protocols.contains(protocol);
}

QStringList CProtocolManager::getApplicationsForProtocol(const QString &protocol)
{
    QStringList apps;

#ifdef Q_OS_LINUX
    // 扫描 .desktop 文件
    QStringList desktopPaths = {
        QDir::homePath() + "/.local/share/applications",
        "/usr/share/applications",
        "/usr/local/share/applications"
    };

    for (const QString &path : desktopPaths) {
        QDir dir(path);
        if (dir.exists()) {
            QStringList desktopFiles = dir.entryList(QStringList() << "*.desktop");
            for (const QString &file : desktopFiles) {
                QFile desktopFile(dir.absolutePath() + "/" + file);
                if (desktopFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QTextStream stream(&desktopFile);
                    QString line;
                    while (stream.readLineInto(&line)) {
                        if (line.startsWith("MimeType=")) {
                            if (line.contains("x-scheme-handler/" + protocol)) {
                                apps.append(file);
                                break;
                            }
                        }
                    }
                    desktopFile.close();
                }
            }
        }
    }
#endif

    return apps;
}

QMap<QString, QStringList> CProtocolManager::getAllProtocolsWithApps()
{
    QMap<QString, QStringList> result;
    QStringList protocols = getAllRegisteredProtocols();

    for (const QString &protocol : protocols) {
        QStringList apps = getApplicationsForProtocol(protocol);
        if (!apps.isEmpty()) {
            result[protocol] = apps;
        }
    }

    return result;
}

void CProtocolManager::registerAllProcotol(QWebEngineProfile *profile)
{
    QStringList protocols = getAllRegisteredProtocols();
    // 过滤掉常见的内置协议，避免冲突
    QStringList excludeProtocols = {
        "http", "https", "ftp", "file", "data",
        "javascript", "about", "qrc", "blob"
    };
    foreach (const QString &protocol, protocols) {
        // 跳过已排除的协议
        if (excludeProtocols.contains(protocol)) {
            qDebug(log) << "Skipping built-in protocol:" << protocol;
            continue;
        }
        QString szHandler = getDefaultHandlerForProtocol(protocol);
        if(szHandler.isEmpty()) {
            qDebug(log) << "Skipping protocol with no handler:" << protocol;
            continue;
        }

        try {
            // 1. 注册协议方案（使用小写）
            QString lowerProtocol = protocol.toLower();
            // 1. 注册协议方案 - 修正1: 使用 QByteArray 而不是 QString
            QByteArray schemeName = lowerProtocol.toLatin1();
            QWebEngineUrlScheme scheme(schemeName);
            scheme.setFlags(QWebEngineUrlScheme::LocalScheme |
                            QWebEngineUrlScheme::LocalAccessAllowed);

            // 注意：重复注册会导致警告，可以先检查是否已注册
            if(QWebEngineUrlScheme::schemeByName(schemeName).name().isEmpty())
                QWebEngineUrlScheme::registerScheme(scheme);
            else {
                qDebug(log) << schemeName << "is registed";
                continue;
            }

            // 2. 安装处理器
            auto *handler = new CSystemProtocolHandler(schemeName, profile);
            profile->installUrlSchemeHandler(schemeName, handler);

        } catch (const std::exception &e) {
            qWarning(log) << "Exception while registering protocol:" << protocol << e.what();
        }
    }
}