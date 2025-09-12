// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QElapsedTimer>
#include <QFileSystemWatcher>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QWebEngineDownloadRequest>
#else
#include <QWebEngineDownloadItem>
#define QWebEngineDownloadRequest QWebEngineDownloadItem
#endif

class CFrmDownload : public QWidget
{
    Q_OBJECT
public:
    explicit CFrmDownload(QWebEngineDownloadRequest* downalod, QWidget *parent = nullptr);
    virtual ~CFrmDownload();

Q_SIGNALS:
    // This signal is emitted when the user indicates that they want to remove
    // this download from the downloads list.
    void sigRemoveClicked(CFrmDownload *self);

private slots:
    void slotUpdateWidget();
    void slotCustomContextMenuRequested(const QPoint &pos);
private:
    QLabel* m_pTitle;
    QPushButton* m_pButton;
    QLabel* m_pUrl;
    QProgressBar* m_pProgressBar;
    QLabel* m_pFileInfo;

    QWebEngineDownloadRequest* m_pDownload;

    QElapsedTimer m_timeAdded;
    QFileSystemWatcher m_FileWatcher;
};
