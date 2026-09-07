#pragma once

#include <QWebEngineUrlScheme>
#include <QWebEngineUrlSchemeHandler>

// 通用协议处理器 - 转发到系统默认处理
class CSystemProtocolHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT
public:
    explicit CSystemProtocolHandler(const QString &scheme, QObject *parent = nullptr)
        : QWebEngineUrlSchemeHandler(parent), m_scheme(scheme) {}
protected:
    void requestStarted(QWebEngineUrlRequestJob *request) override;
    QByteArray generateResponsePage(const QUrl &url, bool opened) const;
private:
    bool isUrlSafe(const QUrl &url) const;
private:
    QString m_scheme;
};
