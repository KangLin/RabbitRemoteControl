// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include <QScrollArea>
#include <QUrl>
#include "FrmDownloadManager.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Download.Manager")
CFrmDownloadManager::CFrmDownloadManager(QWidget *parent)
    : QWidget{parent}
    , m_nCount(0)
{
    setWindowTitle(tr("Download manager"));
    auto layout = new QVBoxLayout(this);
    setLayout(layout);
    auto scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    layout->addWidget(scroll);
    QWidget *pWin = new QWidget(this);
    scroll->setWidget(pWin);
    m_pItems = new QVBoxLayout(this);
    pWin->setLayout (m_pItems);
    m_pItems->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
    resize(600, 300);
}

CFrmDownloadManager::~CFrmDownloadManager()
{
    qDebug(log) << Q_FUNC_INFO;
}

void CFrmDownloadManager::slotDownloadRequested(QWebEngineDownloadRequest *download)
{
    Q_ASSERT(download && download->state() == QWebEngineDownloadRequest::DownloadRequested);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    qDebug(log) << "slotDownloadRequested:" << download->downloadDirectory() << download->downloadFileName() << download->url();
#else
    qDebug(log) << "slotDownloadRequested:" << download->path() << download->url();
#endif
    auto pDownload = new CFrmDownload(download);
    Add(pDownload);
    show();
}

int CFrmDownloadManager::Add(CFrmDownload *item)
{
    if(!item || !m_pItems) return 0;
    bool check = connect(item, &CFrmDownload::sigRemoveClicked, this, &CFrmDownloadManager::Remove);
    Q_ASSERT(check);
    m_pItems->insertWidget(0, item, 0, Qt::AlignTop);
    m_nCount++;
    return 0;
}

int CFrmDownloadManager::Remove(CFrmDownload *item)
{
    if(!item || !m_pItems) return 0;
    m_pItems->removeWidget(item);
    item->deleteLater();
    m_nCount--;
    return 0;
}

void CFrmDownloadManager::showEvent(QShowEvent *event)
{
    emit sigVisible(true);
}

void CFrmDownloadManager::hideEvent(QHideEvent *event)
{
    emit sigVisible(false);
}
