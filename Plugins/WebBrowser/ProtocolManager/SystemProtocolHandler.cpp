#include <QFileInfo>
#include <QMessageBox>
#include <QUrl>
#include <QWebEngineUrlRequestJob>
#include <QLoggingCategory>
#include <QDesktopServices>
#include <QBuffer>
#include "ProtocolManager.h"
#include "SystemProtocolHandler.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.SystemProtocolHandler")
void CSystemProtocolHandler::requestStarted(QWebEngineUrlRequestJob *request)
{
    QUrl url = request->requestUrl();
    qDebug(log) << "Intercepting" << m_scheme << ":" << url.toString();

    // 检查 URL 是否有效
    if (!url.isValid()) {
        qWarning(log) << "Invalid URL:" << url;
        request->fail(QWebEngineUrlRequestJob::UrlInvalid);
        return;
    }

    // 安全验证：防止恶意 URL
    if (!isUrlSafe(url)) {
        qWarning(log) << "Blocked potentially unsafe URL:" << url;
        request->fail(QWebEngineUrlRequestJob::UrlInvalid);
        return;
    }

    CProtocolManager mgr;
    QString szHandler = mgr.getDefaultHandlerForProtocol(url.scheme());
    if(szHandler.isEmpty()) {
        qCritical(log) << "Don't find handler" << url.scheme();
        return;
    }
    szHandler = szHandler.section(' ', 0, 0).trimmed();
    int nRet = QMessageBox::question(
        nullptr, url.scheme(),
        tr("Use the %1 to open %2")
            .arg(szHandler)
            .arg(url.toString()),
        QMessageBox::Yes|QMessageBox::No,
        QMessageBox::No);
    if(QMessageBox::Yes == nRet) {
        // 方法1：使用 QDesktopServices 打开（会触发系统默认处理）
        QDesktopServices::openUrl(url);

        // 方法2：直接重定向到外部（如果需要）
        // 注意：这不会在 WebView 中显示，而是打开外部应用
    }

    QByteArray emptyData = generateResponsePage(url, QMessageBox::Yes == nRet);
    QBuffer *buffer = new QBuffer();
    buffer->setData(emptyData);
    buffer->open(QIODevice::ReadOnly);
    request->reply("text/html", buffer);
}

bool CSystemProtocolHandler::isUrlSafe(const QUrl &url) const
{
    // 安全检查：防止 file:// 等危险协议
    QString scheme = url.scheme();
    if (scheme == "file" || scheme == "data") {
        if(url.isLocalFile()) {
            QFileInfo fi(url.toLocalFile());
            if(fi.suffix() == "rrc")
                return true;
            else
                return false;
        }
        // 只允许特定路径
        if (scheme == "file" && !url.path().startsWith("/tmp/")) {
            return false;
        }
        return false;
    }

    // 检查是否包含可疑字符
    QString urlStr = url.toString();
    if (urlStr.contains("..") || urlStr.contains("%2e%2e")) {
        return false;
    }

    return true;
}

QByteArray CSystemProtocolHandler::generateResponsePage(const QUrl &url, bool opened) const
{
    QString html = R"(
            <html>
                <head>
                    <meta charset="UTF-8">
                    <style>
                        body { font-family: Arial, sans-serif; margin: 50px; }
                        .success { color: #4CAF50; }
                        .error { color: #f44336; }
                    </style>
                </head>
                <body>
                    <h2>协议处理</h2>
                    <p>协议: %1</p>
                    <p>URL: %2</p>
                    <p class="%3">%4: %5</p>
                    <p><a href='javascript:history.back()'>%6</a></p>
                </body>
            </html>
        )";

    html = html.arg(url.scheme())
               .arg(url.toString().toHtmlEscaped())
               .arg(opened ? "success" : "error")
               .arg("Status")
               .arg(opened ? "External application opened" : "Cancel Open")
               .arg("Go back");

    return html.toUtf8();
}
