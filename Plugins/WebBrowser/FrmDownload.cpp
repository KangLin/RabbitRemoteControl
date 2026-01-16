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
#include "ui_FrmDownload.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Download")
CFrmDownload::CFrmDownload(QWebEngineDownloadRequest *downalod, QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::CFrmDownload)
    , m_pDownload(downalod)
{
    bool check = false;
    qDebug(log) << Q_FUNC_INFO;
    ui->setupUi(this);
    ui->pbButton->setText(QString());
    ui->lbFileInfo->hide();

    setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(this, &QFrame::customContextMenuRequested,
                    this, &CFrmDownload::slotCustomContextMenuRequested);
    Q_ASSERT(check);

    if(m_pDownload) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        ui->lbTitle->setText(m_pDownload->downloadFileName());
#else
        QFileInfo fi(m_pDownload->path());
        ui->lbTitle->setText(fi.fileName());
#endif
        ui->progressBar->setValue(0);

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
    delete ui;
}

void CFrmDownload::on_pbButton_clicked()
{
    switch(m_pDownload->state()) {
    case QWebEngineDownloadRequest::DownloadRequested:
    case QWebEngineDownloadRequest::DownloadInProgress: {
        m_pDownload->cancel();
        break;
    }
    case QWebEngineDownloadRequest::DownloadCancelled:
    case QWebEngineDownloadRequest::DownloadInterrupted: {
        m_pDownload->resume();
        ui->lbFileInfo->hide();
        break;
    }
    case QWebEngineDownloadRequest::DownloadCompleted: {
        RabbitCommon::CTools::LocateFileWithExplorer(
            QDir(m_pDownload->downloadDirectory()).absoluteFilePath(m_pDownload->downloadFileName()));
        break;
    }
    default: {
        emit sigRemoveClicked(this);
    }
    }
}

void CFrmDownload::slotUpdateWidget()
{
    if(!m_pDownload) return;
    qreal totalBytes = m_pDownload->totalBytes();
    qreal receivedBytes = m_pDownload->receivedBytes();
    qreal bytesPerSecond = 0;  // Initialized to 0 for a reasonable default value
    int remaining = 0;
    QString szRemaining;
    // Check for division by zero
    if (m_timeAdded.elapsed() != 0)
        bytesPerSecond = receivedBytes / m_timeAdded.elapsed() * 1000;

    remaining = (totalBytes - receivedBytes) / bytesPerSecond + 1;
    if(remaining >= 0) {
        QTime tm(0, 0, 0);
        szRemaining = tm.addSecs(remaining).toString(
            QLocale::system().timeFormat(QLocale::ShortFormat));
        qDebug(log) << "Remaining:" << remaining << tm << szRemaining;
    }

    auto state = m_pDownload->state();
    switch (state) {
    case QWebEngineDownloadRequest::DownloadRequested:
        ui->progressBar->setDisabled(false);
        m_pDownload->accept();
        break;
    case QWebEngineDownloadRequest::DownloadInProgress:
        if (totalBytes > 0) {
            ui->progressBar->setValue(qRound(100 * receivedBytes / totalBytes));
            ui->progressBar->setDisabled(false);
            ui->progressBar->setFormat(
                tr("%p% - %1 of %2 downloaded - %3/s - time left: %4")
                    .arg(CStats::Convertbytes(receivedBytes), CStats::Convertbytes(totalBytes),
                         CStats::Convertbytes(bytesPerSecond), szRemaining));
        } else {
            ui->progressBar->setValue(0);
            ui->progressBar->setDisabled(false);
            ui->progressBar->setFormat(
                tr("unknown size - %1 downloaded - %2/s")
                    .arg(CStats::Convertbytes(receivedBytes), CStats::Convertbytes(bytesPerSecond)));
        }
        break;
    case QWebEngineDownloadRequest::DownloadCompleted:
        ui->progressBar->setValue(100);
        ui->progressBar->setDisabled(true);
        ui->progressBar->setFormat(
            tr("completed - %1 downloaded - %2/s")
                .arg(CStats::Convertbytes(receivedBytes), CStats::Convertbytes(bytesPerSecond)));
        break;
    case QWebEngineDownloadRequest::DownloadCancelled:
        ui->progressBar->setValue(0);
        ui->progressBar->setDisabled(true);
        ui->progressBar->setFormat(
            tr("cancelled - %1 downloaded - %2/s")
                .arg(CStats::Convertbytes(receivedBytes), CStats::Convertbytes(bytesPerSecond)));
        break;
    case QWebEngineDownloadRequest::DownloadInterrupted:
        ui->progressBar->setValue(0);
        ui->progressBar->setDisabled(true);
        ui->progressBar->setFormat(
            tr("interrupted: %1")
                .arg(m_pDownload->interruptReasonString()));
        break;
    }

    switch(state) {
    case QWebEngineDownloadRequest::DownloadRequested:
    case QWebEngineDownloadRequest::DownloadInProgress: {
        static QIcon cancelIcon(QIcon::fromTheme("media-playback-stop"));
        ui->pbButton->setIcon(cancelIcon);
        ui->pbButton->setToolTip(tr("Stop downloading"));
        break;
    }
    case QWebEngineDownloadRequest::DownloadCancelled:
    case QWebEngineDownloadRequest::DownloadInterrupted: {
        static QIcon cancelIcon(QIcon::fromTheme("view-refresh"));
        ui->pbButton->setIcon(cancelIcon);
        ui->pbButton->setToolTip(tr("Resumes downloading"));
        ui->lbFileInfo->setText(m_pDownload->interruptReasonString());
        ui->lbFileInfo->show();
        break;
    }
    case QWebEngineDownloadRequest::DownloadCompleted: {
        static QIcon cancelIcon(QIcon::fromTheme("folder-open"));
        ui->pbButton->setIcon(cancelIcon);
        ui->pbButton->setToolTip(tr("Show in folder"));
        ui->progressBar->hide();
        ui->lbFileInfo->setText(tr("Completed") + " - "
                                + CStats::Convertbytes(m_pDownload->totalBytes()));
        ui->lbFileInfo->show();
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
                        ui->lbFileInfo->setText(tr("The file has been deleted."));
                        ui->lbFileInfo->show();
                        ui->pbButton->hide();
                    }
                });
        break;
    }
    }
}

void CFrmDownload::slotCustomContextMenuRequested(const QPoint &pos)
{
    qDebug(log) << Q_FUNC_INFO;
    QString szFile;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QDir d(m_pDownload->downloadDirectory());
    szFile = d.absoluteFilePath(m_pDownload->downloadFileName());
#else
    szFile = m_pDownload->path();
#endif
    QFile downloadFile(szFile);
    QMenu menu(this);
    if(downloadFile.exists()) {
        menu.addAction(QIcon::fromTheme("folder-open"), tr("Show in folder"),
                       this, [this, szFile](){
                           if(!m_pDownload) return;
                           RabbitCommon::CTools::LocateFileWithExplorer(
                               QDir(m_pDownload->downloadDirectory()).filePath(m_pDownload->downloadFileName()));
                       });
        menu.addAction(QIcon::fromTheme("file-open"), tr("Open the file with the associated program"),
                       this, [this, szFile](){
                           if(!m_pDownload) return;
                           QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
                       });
    }
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
    if(downloadFile.exists()) {
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
    }
    QPoint p = pos;
    QWidget* pW = qobject_cast<QWidget*>(sender());
    if(pW)
        p = pW->mapToGlobal(pos);
    menu.exec(p);
}

void CFrmDownload::mouseDoubleClickEvent(QMouseEvent *event)
{
    qDebug(log) << Q_FUNC_INFO;
    if(!m_pDownload) return;
    QString szFile;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QDir d(m_pDownload->downloadDirectory());
    szFile = d.absoluteFilePath(m_pDownload->downloadFileName());
#else
    szFile = m_pDownload->path();
#endif
    QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
}

void CFrmDownload::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug(log) << Q_FUNC_INFO;
    emit sigSelected(this);
}
