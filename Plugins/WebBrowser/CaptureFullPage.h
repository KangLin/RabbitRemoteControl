// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QWebEngineView>

class CCaptureFullPage : public QObject
{
    Q_OBJECT
public:
    explicit CCaptureFullPage(QObject *parent = nullptr);

    void Start(QWebEngineView* view, QUrl url, QString szFile);

Q_SIGNALS:
    void sigFinished();

private Q_SLOTS:
    void stepInit();
    void stepScroll();
    void stepGrab();
    void composeImage();

private:
    QWebEngineView* m_view;
    int m_originY = 0;
    int m_totalHeight = 0;
    int m_viewportHeight = 0;
    int m_deltaY = 0;
    int m_currentY = 0;
    QVector<QImage> m_images;
    QString m_szFile;
};
