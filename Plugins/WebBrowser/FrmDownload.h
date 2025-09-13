// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QFrame>
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

namespace Ui {
class CFrmDownload;
}

class CFrmDownload : public QFrame
{
    Q_OBJECT

public:
    explicit CFrmDownload(QWebEngineDownloadRequest* downalod, QWidget *parent = nullptr);
    ~CFrmDownload();

Q_SIGNALS:
    // This signal is emitted when the user indicates that they want to remove
    // this download from the downloads list.
    void sigRemoveClicked(CFrmDownload *self);
    void sigSelected(CFrmDownload* item);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void slotUpdateWidget();
    void slotCustomContextMenuRequested(const QPoint &pos);

    void on_pbButton_clicked();

private:
    Ui::CFrmDownload *ui;
    QWebEngineDownloadRequest* m_pDownload;

    QElapsedTimer m_timeAdded;
    QFileSystemWatcher m_FileWatcher;
};

