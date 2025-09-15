// Author: Kang Lin <kl222@126.com>

#include <QFileDialog>
#include <QLoggingCategory>
#include <QScrollArea>
#include <QUrl>
#include <QFileInfo>
#include <QDir>
#include "FrmDownloadManager.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Download.Manager")
CFrmDownloadManager::CFrmDownloadManager(CParameterWebBrowser *para, QWidget *parent)
    : QWidget{parent}
    , m_nCount(0)
    , m_pItems(nullptr)
    , m_pPara(para)
{
    setWindowTitle(tr("Download manager"));
    auto layout = new QVBoxLayout(this);
    setLayout(layout);
    auto scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    layout->addWidget(scroll);
    QWidget *pWin = new QWidget(scroll);
    scroll->setWidget(pWin);
    m_pItems = new QVBoxLayout(pWin);
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
    qDebug(log) << Q_FUNC_INFO;
    Q_ASSERT(download && download->state() == QWebEngineDownloadRequest::DownloadRequested);
    QString szPath;
    szPath = m_pPara->GetDownloadFolder();
    if(m_pPara->GetShowDownloadLocation()) {
        szPath = QFileDialog::getExistingDirectory(this, tr("Save as ......"), szPath);
        if(szPath.isEmpty())
            return;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    download->setDownloadDirectory(szPath);
    qDebug(log) << "slotDownloadRequested:" << download->downloadDirectory() << download->downloadFileName() << download->url();
#else
    QFileInfo fi(download->path());
    download->setPath(szPath + QDir::separator() + fi.fileName());
    qDebug(log) << "slotDownloadRequested:" << download->path() << download->url();
#endif
    auto pDownload = new CFrmDownload(download);
    Add(pDownload);
    if(m_pPara) {
        if(m_pPara->GetShowDownloadManager()) {
            show();
            activateWindow();
        }
    }
}

void CFrmDownloadManager::Add(CFrmDownload *item)
{
    if(!item || !m_pItems) return;
    bool check = connect(item, &CFrmDownload::sigRemoveClicked, this, &CFrmDownloadManager::Remove);
    Q_ASSERT(check);
    check = connect(item, &CFrmDownload::sigSelected, this, &CFrmDownloadManager::slotSelected);
    Q_ASSERT(check);
    m_pItems->insertWidget(0, item, 0, Qt::AlignTop);
    m_nCount++;
}

void CFrmDownloadManager::Remove(CFrmDownload *item)
{
    if(!item || !m_pItems) return;
    m_pItems->removeWidget(item);
    item->deleteLater();
    m_nCount--;
}

void CFrmDownloadManager::slotSelected(CFrmDownload *item)
{
    for(int i = 0; i < m_pItems->count(); i++)
    {
        auto it = m_pItems->itemAt(i);
        if(!it) continue;
        auto w = it->widget();
        if(!w) continue;
        if(w == item) {
            item->setFrameShadow(QFrame::Sunken);
        }
    }
    item->setFrameShadow(QFrame::Raised);
}

void CFrmDownloadManager::showEvent(QShowEvent *event)
{
    emit sigVisible(true);
}

void CFrmDownloadManager::hideEvent(QHideEvent *event)
{
    emit sigVisible(false);
}
