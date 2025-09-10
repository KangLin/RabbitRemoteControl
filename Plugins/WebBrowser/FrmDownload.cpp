// Author: Kang Lin <kl222@126.com>

#include <QContextMenuEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLoggingCategory>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include "FrmDownload.h"
#include "Stats.h"
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Download")
CFrmDownload::CFrmDownload(QWebEngineDownloadRequest* downalod, QWidget *parent)
    : QWidget{parent}
    , m_pDownload(downalod)
{
    bool check = false;
    qDebug(log) << Q_FUNC_INFO;

    auto layout = new QVBoxLayout(this);
    setLayout(layout);
    auto hLayout = new QHBoxLayout(this);
    layout->addLayout(hLayout);
    m_pTitle = new QLabel(tr("Title"), this);
    m_pTitle->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_pTitle->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(m_pTitle, &QLabel::customContextMenuRequested,
                    this, &CFrmDownload::slotCustomContextMenuRequested);
    Q_ASSERT(check);
    hLayout->addWidget(m_pTitle);
    m_pButton = new QPushButton(this);
    m_pButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    hLayout->addWidget(m_pButton);
    m_pUrl = new QLabel(tr("Url"), this);
    m_pUrl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_pUrl->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(m_pUrl, &QLabel::customContextMenuRequested,
                    this, &CFrmDownload::slotCustomContextMenuRequested);
    Q_ASSERT(check);
    m_pUrl->hide();
    layout->addWidget(m_pUrl);
    m_pProgressBar = new QProgressBar(this);
    m_pProgressBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    m_pProgressBar->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(m_pProgressBar, &QLabel::customContextMenuRequested,
                    this, &CFrmDownload::slotCustomContextMenuRequested);
    Q_ASSERT(check);
    layout->addWidget(m_pProgressBar);
    m_pFileInfo = new QLabel(this);
    layout->addWidget(m_pFileInfo);
    m_pFileInfo->hide();
    m_pFileInfo->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(m_pFileInfo, &QLabel::customContextMenuRequested,
                    this, &CFrmDownload::slotCustomContextMenuRequested);
    Q_ASSERT(check);

    if(m_pDownload) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        m_pTitle->setText(m_pDownload->downloadFileName());
#else
        QFileInfo fi(m_pDownload->path());
        m_pTitle->setText(fi.fileName());
#endif
        m_pUrl->setText(m_pDownload->url().toString());
        m_pProgressBar->setValue(0);
        check = connect(m_pButton, &QPushButton::clicked,
                        [this](bool) {
                            switch(m_pDownload->state()) {
                            case QWebEngineDownloadRequest::DownloadRequested:
                            case QWebEngineDownloadRequest::DownloadInProgress: {
                                m_pDownload->cancel();
                                break;
                            }
                            case QWebEngineDownloadRequest::DownloadCancelled:
                            case QWebEngineDownloadRequest::DownloadInterrupted: {
                                m_pDownload->resume();
                                break;
                            }
                            case QWebEngineDownloadRequest::DownloadCompleted: {
                                RabbitCommon::CTools::LocateFileWithExplorer(
                                    QDir(m_pDownload->downloadDirectory()).filePath(m_pDownload->downloadFileName()));
                                // QDesktopServices::openUrl(QUrl::fromLocalFile(
                                //     m_pDownload->downloadDirectory()));
                                break;
                            }
                            default: {
                                emit sigRemoveClicked(this);
                            }
                            }
                        });
        Q_ASSERT(check);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        check = connect(m_pDownload, &QWebEngineDownloadRequest::totalBytesChanged, this, &CFrmDownload::slotUpdateWidget);
        Q_ASSERT(check);
        check = connect(m_pDownload, &QWebEngineDownloadRequest::receivedBytesChanged, this, &CFrmDownload::slotUpdateWidget);
        Q_ASSERT(check);
#else
        check = connect(m_pDownload, &QWebEngineDownloadRequest::downloadProgress, this, &CFrmDownload::slotUpdateWidget);
        Q_ASSERT(check);
#endif
        check = connect(m_pDownload, &QWebEngineDownloadRequest::stateChanged,
                        this, &CFrmDownload::slotUpdateWidget);
        Q_ASSERT(check);
    }

    m_timeAdded.start();
    slotUpdateWidget();
}

CFrmDownload::~CFrmDownload()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CFrmDownload::slotUpdateWidget()
{
    if(!m_pDownload) return;
    qreal totalBytes = m_pDownload->totalBytes();
    qreal receivedBytes = m_pDownload->receivedBytes();
    qreal bytesPerSecond = 0;  // Initialized to 0 for a reasonable default value
    
    // Check for division by zero
    if (m_timeAdded.elapsed() != 0)
        bytesPerSecond = receivedBytes / m_timeAdded.elapsed() * 1000;
    
    auto state = m_pDownload->state();
    switch (state) {
    case QWebEngineDownloadRequest::DownloadRequested:
        m_pDownload->accept();
        break;
    case QWebEngineDownloadRequest::DownloadInProgress:
        if (totalBytes > 0) {
            m_pProgressBar->setValue(qRound(100 * receivedBytes / totalBytes));
            m_pProgressBar->setDisabled(false);
            m_pProgressBar->setFormat(
                tr("%p% - %1 of %2 downloaded - %3/s")
                    .arg(CStats::Convertbytes(receivedBytes), CStats::Convertbytes(totalBytes),
                         CStats::Convertbytes(bytesPerSecond)));
        } else {
            m_pProgressBar->setValue(0);
            m_pProgressBar->setDisabled(false);
            m_pProgressBar->setFormat(
                tr("unknown size - %1 downloaded - %2/s")
                    .arg(CStats::Convertbytes(receivedBytes), CStats::Convertbytes(bytesPerSecond)));
        }
        break;
    case QWebEngineDownloadRequest::DownloadCompleted:
        m_pProgressBar->setValue(100);
        m_pProgressBar->setDisabled(true);
        m_pProgressBar->setFormat(
            tr("completed - %1 downloaded - %2/s")
                .arg(CStats::Convertbytes(receivedBytes), CStats::Convertbytes(bytesPerSecond)));
        break;
    case QWebEngineDownloadRequest::DownloadCancelled:
        m_pProgressBar->setValue(0);
        m_pProgressBar->setDisabled(true);
        m_pProgressBar->setFormat(
            tr("cancelled - %1 downloaded - %2/s")
                .arg(CStats::Convertbytes(receivedBytes), CStats::Convertbytes(bytesPerSecond)));
        break;
    case QWebEngineDownloadRequest::DownloadInterrupted:
        m_pProgressBar->setValue(0);
        m_pProgressBar->setDisabled(true);
        m_pProgressBar->setFormat(
            tr("interrupted: %1")
                .arg(m_pDownload->interruptReasonString()));
        break;
    }
    switch(state) {
    case QWebEngineDownloadRequest::DownloadRequested:
    case QWebEngineDownloadRequest::DownloadInProgress: {
        static QIcon cancelIcon(QIcon::fromTheme("media-playback-stop"));
        m_pButton->setIcon(cancelIcon);
        m_pButton->setToolTip(tr("Stop downloading"));
        break;
    }
    case QWebEngineDownloadRequest::DownloadCancelled:
    case QWebEngineDownloadRequest::DownloadInterrupted: {
        static QIcon cancelIcon(QIcon::fromTheme("view-refresh"));
        m_pButton->setIcon(cancelIcon);
        m_pButton->setToolTip(tr("Resumes downloading"));
        break;
    }
    case QWebEngineDownloadRequest::DownloadCompleted: {
        static QIcon cancelIcon(QIcon::fromTheme("folder-open"));
        m_pButton->setIcon(cancelIcon);
        m_pButton->setToolTip(tr("Show in folder"));
        m_pProgressBar->hide();
        m_pFileInfo->setText(tr("Completed") + " - "
                             + CStats::Convertbytes(m_pDownload->totalBytes()));
        m_pFileInfo->show();
        QString szFile;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QDir d(m_pDownload->downloadDirectory());
        szFile = d.absoluteFilePath(m_pDownload->downloadFileName());
#else
        szFile = m_pDownload->path();
#endif
        m_FileWatcher.addPath(szFile);
        connect(&m_FileWatcher, &QFileSystemWatcher::fileChanged,
                [&](const QString &path) {
            QFile f(path);
            if(!f.exists()) {
                m_pFileInfo->setText(tr("The file has been deleted."));
                m_pFileInfo->show();
                m_pButton->hide();
            }
        });
        break;
    }
    }
}

void CFrmDownload::slotCustomContextMenuRequested(const QPoint &pos)
{
    qDebug(log) << Q_FUNC_INFO;
    QMenu menu(this);
    menu.addAction(QIcon::fromTheme("edit-copy"), tr("Copy url to clipboard"),
                   this, [&](){
        if(m_pDownload && QApplication::clipboard()) {
            QApplication::clipboard()->setText(m_pDownload->url().toString());
        }
    });
    menu.addAction(QIcon::fromTheme("list-remove"), tr("Remove from list"),
                   this, [&](){
        emit sigRemoveClicked(this);
    });
    menu.addAction(QIcon::fromTheme("edit-delete"), tr("Delete"),
                   this, [&](){
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QDir d(m_pDownload->downloadDirectory());
        if(!d.remove(m_pDownload->downloadFileName()))
            qCritical(log) << "Remove file fail." << d.absoluteFilePath(m_pDownload->downloadFileName());
#else
        QFile f(m_pDownload->path());
        if(!f.remove())
            qCritical(log) << "Remove file fail." << f.fileName();
#endif
        emit sigRemoveClicked(this);
    });
    QPoint p = pos;
    QWidget* pW = qobject_cast<QWidget*>(sender());
    if(pW)
        p = pW->mapToGlobal(pos);
    menu.exec(p);
}
