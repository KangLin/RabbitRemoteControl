#pragma once

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QUrl>
#include <QWebEngineProfile>

class CProtocolManager : public QObject
{
    Q_OBJECT
public:
    explicit CProtocolManager(QObject *parent = nullptr);

    // 获取所有已注册的协议
    QStringList getAllRegisteredProtocols();

    // 获取特定协议的默认处理程序
    QString getDefaultHandlerForProtocol(const QString &protocol);

    // 检查协议是否已注册
    bool isProtocolRegistered(const QString &protocol);

    // 获取协议关联的应用列表
    QStringList getApplicationsForProtocol(const QString &protocol);

    // 获取所有协议及其关联应用
    QMap<QString, QStringList> getAllProtocolsWithApps();

    void registerAllProcotol(QWebEngineProfile* profile = QWebEngineProfile::defaultProfile());

private:
    // 平台特定的实现
    QStringList getRegisteredProtocolsLinux();
    QStringList getRegisteredProtocolsWindows();
    QStringList getRegisteredProtocolsMacOS();
    void scanDesktopFilesForProtocols(QStringList &protocols);
    void parseDesktopFile(const QString &filePath, QStringList &protocols);

    // 辅助方法
    QString getDefaultHandlerLinux(const QString &protocol);
    QString getDefaultHandlerWindows(const QString &protocol);
    QString getDefaultHandlerMacOS(const QString &protocol);
};
