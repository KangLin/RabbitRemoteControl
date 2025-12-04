// Author: Kang Lin <kl222@126.com>

#include <QTimer>
#include <QPainter>
#include <QLoggingCategory>
#include "CaptureFullPage.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Capture.Page")
CCaptureFullPage::CCaptureFullPage(QObject *parent)
    : QObject{parent}
{}

void CCaptureFullPage::Start(QWebEngineView *view, QUrl url, QString szFile)
{
    if(!view || url.isEmpty() || szFile.isEmpty())
        return;
    m_szFile = szFile;
    m_view = view;
    stepInit();
}

void CCaptureFullPage::stepInit()
{
    qDebug(log) << "contents size:" << m_view->page()->contentsSize();
    // 获取页面总高度
    m_totalHeight = m_view->page()->contentsSize().height();
    m_viewportHeight = m_view->size().height();
    m_deltaY = m_viewportHeight;
    m_currentY = 0;
    m_images.clear();
    m_view->page()->runJavaScript(
        "document.documentElement.scrollTop;",
        [this](const QVariant &result) {
            m_originY = result.toInt();
            //qDebug(log) << "OriginY:" << m_originY;
        });
    stepScroll();
}

void CCaptureFullPage::stepScroll()
{
    if (m_currentY >= m_totalHeight) {
        composeImage();
        return;
    }

    // 滚动到当前位置
    m_view->page()->runJavaScript(
        QString("window.scrollTo(0, %1);").arg(m_currentY),
        [this](const QVariant &) {
            // 等待渲染
            QTimer::singleShot(500, this, &CCaptureFullPage::stepGrab);
        }
        );
}

void CCaptureFullPage::stepGrab()
{
    QImage img = m_view->grab().toImage();
    // 如果是最后一块，有可能要裁掉多余的部分
    int expectedHeight = (m_currentY + m_deltaY > m_totalHeight) ? (m_totalHeight - m_currentY) : m_deltaY;
    if (img.height() > expectedHeight) {
        img = img.copy(0, 0, img.width(), expectedHeight);
    }

    m_images.append(img);
    m_currentY += m_deltaY;
    stepScroll(); // 进入下一分块
}

void CCaptureFullPage::composeImage()
{
    if (!m_images.isEmpty()) {
        int fullWidth = m_images[0].width();
        int fullHeight = 0;
        foreach (const QImage &img, m_images) fullHeight += img.height();

        QImage result(fullWidth, fullHeight, m_images[0].format());
        QPainter painter(&result);
        int y = 0;
        foreach (const QImage &img, m_images) {
            painter.drawImage(0, y, img);
            y += img.height();
        }
        painter.end();
        result.save(m_szFile);
        qDebug() << "Full page screenshot saved to" << m_szFile;
    }
    m_view->page()->runJavaScript(
        QString("window.scrollTo(0, %1);").arg(m_originY),
        [this](const QVariant &) {});
    emit sigFinished();
}
