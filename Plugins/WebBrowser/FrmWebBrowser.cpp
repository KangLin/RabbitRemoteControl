// Author: Kang Lin <kl222@126.com>

#include <QPainter>
#include <QTabBar>
#include <QMessageBox>
#include <QMenu>
#include <QFile>
#include <QDir>
#include <QPrinter>
#include <QInputDialog>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QWebEngineHistory>
#include <QWebEngineSettings>
#include <QWebEngineFindTextResult>
#include <QRegularExpression>
#include <QWebEngineCookieStore>
#include <QStandardPaths>
#include <QClipboard>
#include <QApplication>
#include <QDesktopServices>
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    #include <QWebEngineProfileBuilder>
#endif
#include <QLoggingCategory>

#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "FrmPopup.h"
#include "CaptureFullPage.h"
#include "FrmHistory.h"
#include "AddressCompleter.h"
#include "AutoCompleteLineEdit.h"
#include "FrmBookmark.h"
#include "FrmAddBookmark.h"
#include "DlgSettings.h"
#include "FrmWebBrowser.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Browser")
CFrmWebBrowser::CFrmWebBrowser(CParameterWebBrowser *pPara, bool bMenuBar, QWidget *parent)
    : QWidget{parent}
    , m_pPara(pPara)
    , m_pMenuBar(nullptr)
    , m_pToolBar(nullptr)
    , m_pBack(nullptr)
    , m_pForward(nullptr)
    , m_pRefresh(nullptr)
    , m_pStop(nullptr)
    , m_pAddBookmark(nullptr)
    , m_pFind(nullptr)
    , m_pFindNext(nullptr)
    , m_pFindPrevious(nullptr)
    , m_pZoomOriginal(nullptr)
    , m_pZoomIn(nullptr)
    , m_pZoomOut(nullptr)
    , m_pFavAction(nullptr)
    , m_pGo(nullptr)
    , m_pAddPage(nullptr)
    , m_pAddPageIncognito(nullptr)
    , m_pAddWindow(nullptr)
    , m_pPrint(nullptr)
    , m_pPrintToPdf(nullptr)
    , m_pAddWindowIncognito(nullptr)
    , m_pDownload(nullptr)
    , m_pInspector(nullptr)
    , m_pUrl(nullptr)
    , m_pUrlLineEdit(nullptr)
    , m_pProgressBar(nullptr)
    , m_pTab(nullptr)
    , m_DownloadManager(pPara)
    , m_pCapturePage(nullptr)
    , m_pCaptureFulPage(nullptr)
    , m_pRecord(nullptr)
    , m_pMultimediaRecord(nullptr)
    , m_pHistoryDatabase(nullptr)
    , m_pBookmarkDatabase(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    bool check = false;

    QString szDbHistory = GetProfile()->persistentStoragePath();
    if(!(szDbHistory.right(1) == '/' || szDbHistory.right(1) == '\\'))
        szDbHistory += QDir::separator();
    szDbHistory += "History.db";
    m_pHistoryDatabase = CHistoryDatabase::Instance(szDbHistory);

    QString szDbBookmarks = GetProfile()->persistentStoragePath();
    if(!(szDbBookmarks.right(1) == '/' || szDbBookmarks.right(1) == '\\'))
        szDbBookmarks += QDir::separator();
    szDbBookmarks += "Bookmarks.db";
    m_pBookmarkDatabase = CBookmarkDatabase::Instance(m_pHistoryDatabase->GetDatabase());

    setWindowIcon(QIcon::fromTheme("web-browser"));

    QVBoxLayout* pLayout = new QVBoxLayout(this);
    if(!pLayout) {
        return;
    }
    pLayout->setSpacing(0);
    pLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pLayout);

    m_pToolBar = new QToolBar(this);
    if(m_pToolBar) {
        pLayout->addWidget(m_pToolBar);
    }

    m_pBack = m_pToolBar->addAction(
        QIcon::fromTheme("go-previous"), tr("Back"),
        this, [&](){
            CFrmWebView* pWeb = CurrentView();
            if(pWeb && pWeb->page())
                pWeb->page()->action(QWebEnginePage::Back)->trigger();
        });
    m_pBack->setEnabled(false);
    m_pBack->setShortcuts(QKeySequence::Back);
    m_pBack->setStatusTip(m_pBack->text());
    m_pForward = m_pToolBar->addAction(
        QIcon::fromTheme("go-next"), tr("Forward"),
        this, [&](){
            CFrmWebView* pWeb = CurrentView();
            if(pWeb && pWeb->page())
                pWeb->page()->action(QWebEnginePage::Forward)->trigger();
        });
    m_pForward->setEnabled(false);
    m_pForward->setShortcuts(QKeySequence::Forward);
    m_pForward->setStatusTip(m_pForward->text());
    m_pRefresh = m_pToolBar->addAction(
        QIcon::fromTheme("view-refresh"), tr("Refresh"),
        this, [&](){
            CFrmWebView* pWeb = CurrentView();
            if(pWeb && pWeb->page())
                pWeb->page()->action(QWebEnginePage::Reload)->trigger();
        });
    m_pRefresh->setShortcuts(QKeySequence::Refresh);
    m_pRefresh->setStatusTip(m_pRefresh->text());

    m_pUrlLineEdit = new QLineEdit(this);
    m_pFavAction = new QAction(m_pUrlLineEdit);
    m_pUrlLineEdit->addAction(m_pFavAction, QLineEdit::LeadingPosition);
    m_pUrlLineEdit->setClearButtonEnabled(true);
    m_pUrl = m_pToolBar->addWidget(m_pUrlLineEdit);
    m_pGo = new QAction(QIcon::fromTheme("go-next"), tr("go"), m_pUrlLineEdit);
    m_pGo->setStatusTip(m_pGo->text());
    check = connect(m_pGo, &QAction::triggered, this, &CFrmWebBrowser::slotReturnPressed);
    Q_ASSERT(check);
    m_pUrlLineEdit->addAction(m_pGo, QLineEdit::TrailingPosition);
    m_pGo->setVisible(false);
    check = connect(m_pUrlLineEdit, &QLineEdit::textEdited,
                    this, [&](const QString &text){
        QLineEdit* pLineEdit = qobject_cast<QLineEdit*>(sender());
        if(pLineEdit) {
            if(text.isEmpty()) {
                if(m_pGo->isVisible())
                    m_pGo->setVisible(false);
            } else {
                if(!m_pGo->isVisible())
                    m_pGo->setVisible(true);
            }
        }
    });
    Q_ASSERT(check);
    // 创建地址栏自动完成器
    auto pAddressCompleter = new CAddressCompleter(this);
    pAddressCompleter->attachToLineEdit(m_pUrlLineEdit);
    if(m_pPara)
        pAddressCompleter->setMaxVisibleItems(m_pPara->GetAddCompleterLines());
    check = connect(pAddressCompleter, &CAddressCompleter::urlSelected,
                    this, &CFrmWebBrowser::slotUrlSelected);
    Q_ASSERT(check);
    check = connect(pAddressCompleter, &CAddressCompleter::searchRequested,
                    this, [&](const QString& keyword) {
        qDebug(log) << "CAddressCompleter::searchRequested keyword:" << keyword;
        QString szSearch;
        QString szUrl;
        if(m_pPara) {
            szSearch = m_pPara->GetSearchEngine();
            szUrl = szSearch.replace(m_pPara->GetSearchRelaceString(),
                                 QUrl::toPercentEncoding(keyword));
            if(!szUrl.isEmpty())
                slotUrlSelected(szUrl);
        }
    });
    Q_ASSERT(check);
    check = connect(pAddressCompleter, &CAddressCompleter::sigCommand,
                    this, [&](const QString& cmd) {
        if(cmd.startsWith("@setting")) {
            CDlgSettings dlg(m_pPara);
            RC_SHOW_WINDOW(&dlg);
        } else if(cmd.startsWith("@history")) {
            slotHistory();
        } else if(cmd.startsWith("@bookmark")) {
            slotBookmark();
        }
    });
    Q_ASSERT(check);

    m_pAddBookmark = m_pToolBar->addAction(
        QIcon::fromTheme("user-bookmarks"),
        tr("Add bookmark"), this, SLOT(slotAddBookmark()));

    m_pAddPage = m_pToolBar->addAction(QIcon::fromTheme("add"), tr("Add tab page"),
                                   this, [&](){
        CreateWindow(QWebEnginePage::WebBrowserTab);
        if(!m_pPara->GetTabUrl().isEmpty()) {
            m_pUrlLineEdit->setText(m_pPara->GetTabUrl());
            slotReturnPressed();
        }
    });
    m_pAddPage->setStatusTip(m_pAddPage->text());
    m_pAddPage->setShortcuts(QKeySequence::AddTab);
    Q_ASSERT(check);
    m_pDownload = m_pToolBar->addAction(
        QIcon::fromTheme("emblem-downloads"), tr("Download Manager"));
    m_pDownload->setCheckable(true);
    m_pDownload->setStatusTip(m_pDownload->text());
    m_pDownload->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
    check = connect(m_pDownload, &QAction::toggled,
                    this, [&](bool checked){
                        if(checked)
                            m_DownloadManager.show();
                        else
                            m_DownloadManager.hide();
                    });
    Q_ASSERT(check);
    check = connect(&m_DownloadManager, &CFrmDownloadManager::sigVisible, this,
                    [&](bool visible){
                        if(m_pDownload)
                            m_pDownload->setChecked(visible);
    });
    Q_ASSERT(check);

    m_pProgressBar = new QProgressBar(this);
    if(m_pProgressBar) {
        pLayout->addWidget(m_pProgressBar);
        m_pProgressBar->setMaximumHeight(1);
        m_pProgressBar->setTextVisible(false);
        m_pProgressBar->show();
        m_pProgressBar->setStyleSheet("QProgressBar {border: 0px} QProgressBar::chunk {background-color: #da4453}");
    }

    m_pTab = new QTabWidget(this);
    if(m_pTab) {
        m_pTab->setTabsClosable(true);
        m_pTab->setUsesScrollButtons(true);
        m_pTab->setMovable(true);
        m_pTab->setElideMode(Qt::TextElideMode::ElideRight);
        pLayout->addWidget(m_pTab);
    }
    check = connect(m_pTab, &QTabWidget::currentChanged,
                    this, &CFrmWebBrowser::slotTabCurrentChanged);
    Q_ASSERT(check);
    check = connect(m_pTab, &QTabWidget::tabCloseRequested,
                    this, &CFrmWebBrowser::slotTabCloseRequested);
    Q_ASSERT(check);

    m_DownloadManager.hide();
    QWebEngineProfile::defaultProfile()->setDownloadPath(m_pPara->GetDownloadFolder());
    check = connect(QWebEngineProfile::defaultProfile(), &QWebEngineProfile::downloadRequested,
                     &m_DownloadManager, &CFrmDownloadManager::slotDownloadRequested);
    Q_ASSERT(check);
    check = connect(m_pPara, &CParameterWebBrowser::sigDownloadFolderChanged,
                    this, [&](){
        QWebEngineProfile::defaultProfile()->setDownloadPath(m_pPara->GetDownloadFolder());
        if(m_profile)
            m_profile->setDownloadPath(m_pPara->GetDownloadFolder());
    });
    Q_ASSERT(check);

    InitMenu(&m_Menu);
    if(bMenuBar)
    {
        m_pMenuBar = new QMenuBar(this);
        if(m_pMenuBar) {
            m_Menu.setTitle(tr("Operate"));
            m_pMenuBar->addMenu(&m_Menu);
            pLayout->setMenuBar(m_pMenuBar);
        }
    }

    check = connect(&m_tmRecord, &QTimer::timeout, this, &CFrmWebBrowser::slotRecordTimeout);
    Q_ASSERT(check);
}

CFrmWebBrowser::~CFrmWebBrowser()
{
    qDebug(log) << Q_FUNC_INFO;
    if(m_pToolBar) {
        m_pToolBar->deleteLater();
        m_pToolBar = nullptr;
    }
    if(m_pTab) {
        m_pTab->deleteLater();
        m_pTab = nullptr;
    }

    if(m_pPara->GetClearCookie() && m_profile)
        m_profile->cookieStore()->deleteAllCookies();
    if(m_pPara->GetClearHttpCache() && m_profile)
        m_profile->clearHttpCache();
}

QMenu* CFrmWebBrowser::GetMenu(QWidget *parent)
{
    if(m_Menu.actions().isEmpty())
        return nullptr;
    return &m_Menu;
}

QWebEngineView* CFrmWebBrowser::CreateWindow(
    QWebEnginePage::WebWindowType type, bool offTheRecord)
{
    qDebug(log) << "Create window:" << type;

    CFrmWebView* pView = nullptr;
    switch (type) {
    case QWebEnginePage::WebBrowserTab: {
        auto pTab = CreateTab(&pView, offTheRecord);
        if(!pTab || !m_pTab) break;
        int index = m_pTab->addTab(pTab, pView->favIcon(), tr("New page"));
        if(-1 < index)
            m_pTab->setCurrentIndex(index);
        break;
    }
    case QWebEnginePage::WebBrowserBackgroundTab: {
        auto pTab = CreateTab(&pView, offTheRecord);
        if(!pTab || !m_pTab) break;
        int index = m_pTab->currentIndex();
        m_pTab->addTab(pTab, pView->favIcon(), tr("New page"));
        if(-1 < index && index != m_pTab->currentIndex())
            m_pTab->setCurrentIndex(index);
        break;
    }
    case QWebEnginePage::WebBrowserWindow: {
        auto pWin = new CFrmWebBrowser(m_pPara, true);
        if(pWin) {
            pView = qobject_cast<CFrmWebView*>(
                pWin->CreateWindow(QWebEnginePage::WebBrowserTab));
            pWin->setAttribute(Qt::WA_DeleteOnClose);
            auto pMainWin = RabbitCommon::CTools::GetMainWindow();
            if(pMainWin)
                pWin->resize(pMainWin->frameGeometry().width(),
                             pMainWin->frameGeometry().height());
            pWin->show();
        }
        break;
    }
    case QWebEnginePage::WebDialog: {
        // Test: Resource\html\TestQWebEnginePageWebDialog.html
        auto popup = new CFrmPopup(GetProfile(offTheRecord), this);
        pView = popup->GetView();
    }
    }

    return pView;
}

void CFrmWebBrowser::SetConnect(CFrmWebView* pWeb)
{
    bool check = false;
    if(!pWeb) return;
    check = connect(pWeb, &CFrmWebView::sigDevToolsRequested,
                         this, [&](){
                             if(m_pInspector)
                                 m_pInspector->setChecked(true);
                         });
    Q_ASSERT(check);
    check = connect(pWeb, &CFrmWebView::sigWebActionEnabledChanged,
                    this, [this, pWeb](QWebEnginePage::WebAction webAction, bool enabled){
        if(!IsCurrentView(pWeb)) return;
        switch(webAction){
        case QWebEnginePage::WebAction::Back:
            m_pBack->setEnabled(enabled);
            break;
        case QWebEnginePage::WebAction::Forward:
            m_pForward->setEnabled(enabled);
            break;
        case QWebEnginePage::WebAction::Reload: {
            m_pRefresh->setEnabled(enabled);
            if(m_pRefresh->isEnabled())
                m_pToolBar->insertAction(m_pUrl, m_pRefresh);
            else
                m_pToolBar->removeAction(m_pRefresh);
            break;
        }
        case QWebEnginePage::WebAction::Stop: {
            m_pStop->setEnabled(enabled);
            if(m_pStop->isEnabled())
                m_pToolBar->insertAction(m_pUrl, m_pStop);
            else
                m_pToolBar->removeAction(m_pStop);
            break;
        }
        default:
            break;
        }
    });
    check = connect(pWeb, &QWebEngineView::urlChanged,
                    this, [&](const QUrl &url) {
                        CFrmWebView* pWeb = qobject_cast<CFrmWebView*>(sender());
                        if(IsCurrentView(pWeb))
                            m_pUrlLineEdit->setText(url.toString());
                        if(m_pHistoryDatabase) {
                            m_pHistoryDatabase->addHistoryEntry(url.toString());
                        }
                    });
    Q_ASSERT(check);
    check = connect(pWeb, &CFrmWebView::titleChanged,
                    this, [&](const QString &title) {
                        CFrmWebView* pWeb = qobject_cast<CFrmWebView*>(sender());
                        int index = IndexOfTab(pWeb);
                        if(-1 < index) {
                            if(m_pTab)
                                m_pTab->setTabText(index, title);
                            setWindowTitle(title);
                            emit sigUpdateTitle();
                        }
                        if(m_pHistoryDatabase) {
                            m_pHistoryDatabase->updateHistoryEntry(pWeb->url().toString(), title);
                        }
                    });
    Q_ASSERT(check);
    check = connect(pWeb, &CFrmWebView::favIconChanged,
                    this, [&](const QIcon &icon){
                        CFrmWebView* pWeb = qobject_cast<CFrmWebView*>(sender());
                        int index = IndexOfTab(pWeb);
                        if(-1 < index) {
                            if(m_pTab)
                                m_pTab->setTabIcon(index, icon);
                            m_pFavAction->setIcon(icon);
                            setWindowIcon(icon);
                            emit sigUpdateTitle();
                        }
                        if(m_pHistoryDatabase) {
                            m_pHistoryDatabase->updateHistoryEntry(pWeb->url().toString(), QString(), icon);
                        }
                    });
    Q_ASSERT(check);
    check = connect(pWeb, &CFrmWebView::sigLinkHovered,
                    this, &CFrmWebBrowser::sigInformation);
    Q_ASSERT(check);
    check = connect(pWeb, &CFrmWebView::sigCloseRequested,
                    this, &CFrmWebBrowser::slotViewCloseRequested);
    Q_ASSERT(check);
    check = connect(pWeb, &CFrmWebView::loadProgress,
                    this, [&](int progress){
                        CFrmWebView* pWeb = qobject_cast<CFrmWebView*>(sender());
                        if(IsCurrentView(pWeb))
                            m_pProgressBar->setValue(progress);
                    });
    Q_ASSERT(check);
    m_pProgressBar->setValue(pWeb->progress());
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    check = connect(pWeb, &CFrmWebView::printFinished,
                    this, &CFrmWebBrowser::slotPrintFinished);
    Q_ASSERT(check);
    check = connect(pWeb, &CFrmWebView::pdfPrintingFinished,
                    this, &CFrmWebBrowser::slotPdfPrintingFinished);
    Q_ASSERT(check);
#endif
}

QWebEngineProfile* CFrmWebBrowser::GetProfile(bool offTheRecord)
{
    if(offTheRecord)
        return QWebEngineProfile::defaultProfile();
    if(m_profile)
        return m_profile.get();

    QSettings set(RabbitCommon::CDir::Instance()->GetDirUserData()
                      + QDir::separator() + "WebBrowser.ini",
                  QSettings::IniFormat);
    QString name = "io.github.KangLin.RabbitRemoteControl";
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
    name += QLatin1StringView(qWebEngineChromiumVersion());
    name = set.value("Profile/Name", name).toString();
    QWebEngineProfileBuilder profileBuilder;
    m_profile.reset(profileBuilder.createProfile(name));
#else
    name = set.value("Profile/Name", name).toString();
    m_profile.reset(new QWebEngineProfile(name));
#endif
    if(!m_profile)
        return QWebEngineProfile::defaultProfile();

    set.setValue("Profile/Name", name);

    m_profile->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    m_profile->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    m_profile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    m_profile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    m_profile->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    m_profile->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    m_profile->settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
#endif
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    m_profile->settings()->setAttribute(QWebEngineSettings::PlaybackRequiresUserGesture, false);
#endif
    m_profile->setDownloadPath(m_pPara->GetDownloadFolder());
    bool check = connect(m_profile.get(), &QWebEngineProfile::downloadRequested,
                         &m_DownloadManager, &CFrmDownloadManager::slotDownloadRequested);
    Q_ASSERT(check);
    //m_profile->setPersistentCookiesPolicy(QWebEngineProfile::ForcePersistentCookies);
    //m_profile->setPersistentStoragePath(m_profile->cachePath() + QDir::separator() + "Persistent");
    //m_profile->setHttpCacheMaximumSize(50);
    qDebug(log) << "User agent:" << m_profile->httpUserAgent()
#if QT_VERSION > QT_VERSION_CHECK(6, 8, 0)
                //<< "AllPermissions:" << m_profile->listAllPermissions()
                << "persistentPermissionsPolicy:" << m_profile->persistentPermissionsPolicy()
#endif
                << "persistentCookiesPolicy:" << m_profile->persistentCookiesPolicy()
                << "httpCacheMaximumSize:" << m_profile->httpCacheMaximumSize()
                << "Persistent path:" << m_profile->persistentStoragePath()
                << "Cache path:" << m_profile->cachePath()
                << "Storage name:" << m_profile->storageName()
                << "Cookie:" << m_profile->cookieStore()
                << "Is off the Record:" << m_profile->isOffTheRecord()
                << "Download:" << m_profile->downloadPath();
    return m_profile.get();
}

CFrmWebView *CFrmWebBrowser::CreateWebView(bool offTheRecord)
{
    auto pView = new CFrmWebView(this);
    if(pView) {
        auto profile = GetProfile(offTheRecord);
// #if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
//         profile->setPersistentPermissionsPolicy(QWebEngineProfile::PersistentPermissionsPolicy::AskEveryTime);
// #endif
        auto page = new QWebEnginePage(profile, pView);
        pView->setPage(page);
    }
    return pView;
}

QWidget* CFrmWebBrowser::CreateTab(CFrmWebView **view, bool offTheRecord)
{
    QSplitter *pSplitter = new QSplitter(Qt::Vertical, this);
    if(pSplitter) {
        pSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        //pSplitter->setContentsMargins(0, 0, 0, 0);
        CFrmWebView* pWeb = nullptr;
        if(view) {
            if(*view) {
                pWeb = *view;
            } else {
                pWeb = CreateWebView(offTheRecord);
                *view = pWeb;
            }
        } else {
            pWeb = CreateWebView(offTheRecord);
        }
        if(pWeb) {
            pSplitter->addWidget(pWeb);
            SetConnect(pWeb);
        }
    }
    return pSplitter;
}

CFrmWebView *CFrmWebBrowser::CurrentView(ViewType type)
{
    if(!m_pTab) return nullptr;
    auto w = m_pTab->currentWidget();
    if(!w) return nullptr;
    QSplitter *pSplitter = qobject_cast<QSplitter*>(w);
    if(!pSplitter) return nullptr;
    int index = (int)type;
    if(0 > index && pSplitter->count() <= index) return nullptr;
    return qobject_cast<CFrmWebView*>(pSplitter->widget(index));
}

CFrmWebView* CFrmWebBrowser::GetView(int index, ViewType type)
{
    if(0 > index || !m_pTab || m_pTab->count() <= index) return nullptr;
    auto w = m_pTab->widget(index);
    if(!w) return nullptr;
    QSplitter *pSplitter = qobject_cast<QSplitter*>(w);
    if(!pSplitter) return nullptr;
    int idx = (int)type;
    if(0 > idx && pSplitter->count() <= idx) return nullptr;
    return qobject_cast<CFrmWebView*>(pSplitter->widget(idx));
}

bool CFrmWebBrowser::IsCurrentView(CFrmWebView *pView)
{
    if(!m_pTab) return false;
    auto w = m_pTab->currentWidget();
    if(!w) return false;
    QSplitter *pSplitter = qobject_cast<QSplitter*>(w);
    if(!pSplitter) return false;
    return -1 != pSplitter->indexOf(pView);
}

int CFrmWebBrowser::IndexOfTab(CFrmWebView *pView)
{
    int nRet = -1;
    if(!pView) return nRet;
    QWidget* p = qobject_cast<QWidget*>(pView->parent());
    if(!p) return nRet;
    if(m_pTab)
        nRet = m_pTab->indexOf(p);
    return nRet;
}

int CFrmWebBrowser::InitMenu(QMenu *pMenu)
{
    bool check = false;
    if(!pMenu) return 0;
    pMenu->addAction(m_pBack);
    pMenu->addAction(m_pForward);
    pMenu->addAction(m_pRefresh);
    m_pStop = pMenu->addAction(
        QIcon::fromTheme("media-playback-stop"), tr("Stop"), this, [&]() {
            CFrmWebView* pWeb = CurrentView();
            if(pWeb && pWeb->page())
                pWeb->page()->action(QWebEnginePage::Stop)->trigger();
        });
    m_pStop->setEnabled(false);
    m_pStop->setShortcuts(QKeySequence::Cancel);
    m_pStop->setStatusTip(m_pStop->text());

    pMenu->addSeparator();
    pMenu->addAction(m_pAddPage);
    m_pAddPageIncognito = pMenu->addAction(
        QIcon::fromTheme("add"), tr("Add incognito tab"),
        this, [&]() {
            CreateWindow(QWebEnginePage::WebBrowserTab, true);
            if(!m_pPara->GetTabUrl().isEmpty()) {
                m_pUrlLineEdit->setText(m_pPara->GetTabUrl());
                slotReturnPressed();
            }
        });
    m_pAddPageIncognito->setStatusTip(m_pAddPageIncognito->text());
    m_pAddWindow = pMenu->addAction(
        QIcon::fromTheme("add"), tr("Add window"),
        this, [&]() {
            CreateWindow(QWebEnginePage::WebBrowserWindow);
        });
    m_pAddWindow->setVisible(false);
    m_pAddWindow->setStatusTip(m_pAddWindow->text());
    m_pAddWindowIncognito = pMenu->addAction(
        QIcon::fromTheme("add"), tr("Add Incognito Window"),
        this, [&]() {
            CreateWindow(QWebEnginePage::WebBrowserWindow, true);
        });
    m_pAddWindowIncognito->setVisible(false);
    m_pAddWindowIncognito->setStatusTip(m_pAddWindowIncognito->text());

    pMenu->addSeparator();
    m_pFind = pMenu->addAction(
        QIcon::fromTheme("edit-find"), tr("&Find"), this,
        [&]() {
            CFrmWebView* pWeb = CurrentView();
            if(pWeb) {
                bool ok = false;
                if(pWeb->selectedText().isEmpty()) {
                    if(QApplication::clipboard()
                        && !QApplication::clipboard()->text().isEmpty())
                        m_szFindText = QApplication::clipboard()->text();
                } else {
                    m_szFindText = pWeb->selectedText();
                }
                QString search = QInputDialog::getText(
                    this, tr("Find"),
                    tr("Find:"), QLineEdit::Normal,
                    m_szFindText, &ok);
                if (ok && !search.isEmpty()) {
                    m_szFindText = search;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                    pWeb->findText(m_szFindText, QWebEnginePage::FindFlags(),
                                   [&](const QWebEngineFindTextResult& result){
                                       if (result.numberOfMatches() == 0) {
                                           emit sigInformation(tr("\"%1\" not found.").arg(m_szFindText));
                                       } else {
                                           emit sigInformation(tr("\"%1\" found: %2/%3").arg(m_szFindText, QString::number(result.activeMatch()), QString::number(result.numberOfMatches())));
                                       }
                    });
#else
                    pWeb->findText(m_szFindText, QWebEnginePage::FindFlags(), [this](bool found) {
                        if (!found)
                            emit sigInformation(tr("\"%1\" not found.").arg(m_szFindText));
                    });
#endif
                }
            }
        });
    m_pFind->setShortcuts(QKeySequence::Find);
    m_pFind->setStatusTip(m_pFind->text());

    m_pFindNext = pMenu->addAction(
        QIcon::fromTheme("go-next"), tr("Find &Next"), this,
            [this]() {
               CFrmWebView* pWeb = CurrentView();
               if(pWeb && !m_szFindText.isEmpty()) {
                   pWeb->findText(m_szFindText);
               }
        });
    m_pFindNext->setShortcuts(QKeySequence::FindNext);
    m_pFindNext->setText(m_pFindNext->text());

    m_pFindPrevious = pMenu->addAction(
        QIcon::fromTheme("go-previous"), tr("Find &Previous"), this,
        [&]() {
           CFrmWebView* pWeb = CurrentView();
           if(pWeb && !m_szFindText.isEmpty()) {
               pWeb->findText(m_szFindText, QWebEnginePage::FindBackward);
           }
        });
    m_pFindPrevious->setShortcuts(QKeySequence::FindPrevious);
    m_pFindPrevious->setStatusTip(m_pFindPrevious->text());

    pMenu->addSeparator();
    m_pZoomOriginal = pMenu->addAction(
        QIcon::fromTheme("zoom-original"), tr("Original"));
    m_pZoomOriginal->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    m_pZoomOriginal->setStatusTip(tr("Original"));
    m_pZoomOriginal->setToolTip(tr("Original"));
    check = connect(m_pZoomOriginal, &QAction::triggered, this,
                    [&](){
                        CFrmWebView* pWeb = CurrentView();
                        if(pWeb)
                            pWeb->setZoomFactor(1.0);
                    });
    Q_ASSERT(check);
    m_pZoomIn = pMenu->addAction(QIcon::fromTheme("zoom-in"), tr("Zoom in"));
    m_pZoomIn->setShortcuts(QKeySequence::ZoomIn);
    m_pZoomIn->setStatusTip(tr("Zoom in"));
    m_pZoomIn->setToolTip(tr("Zoom in"));
    check = connect(
        m_pZoomIn, &QAction::triggered, this,
        [&](){
            CFrmWebView* pWeb = CurrentView();
            if(pWeb)
                pWeb->setZoomFactor(pWeb->zoomFactor() + 0.1);
        });
    Q_ASSERT(check);
    m_pZoomOut = pMenu->addAction(
        QIcon::fromTheme("zoom-out"), tr("Zoom out"));
    m_pZoomOut->setShortcuts(QKeySequence::ZoomOut);
    m_pZoomOut->setStatusTip(tr("Zoom out"));
    m_pZoomOut->setToolTip(tr("Zoom out"));
    check = connect(
        m_pZoomOut, &QAction::triggered, this,
        [&](){
            CFrmWebView* pWeb = CurrentView();
            if(pWeb)
                pWeb->setZoomFactor(pWeb->zoomFactor() - 0.1);
        });
    Q_ASSERT(check);

    pMenu->addSeparator();
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    m_pPrint = pMenu->addAction(
        QIcon::fromTheme("document-print"), tr("Print"),
        this, &CFrmWebBrowser::slotPrint);
    m_pPrint->setVisible(false);
    m_pPrint->setShortcuts(QKeySequence::Print);
    m_pPrintToPdf = pMenu->addAction(
        QIcon::fromTheme("document-print"), tr("Print to PDF"),
        this, &CFrmWebBrowser::slotPrintToPdf);
#endif

    pMenu->addAction(m_pDownload);
    if(!m_pInspector) {
        m_pInspector = pMenu->addAction(QIcon::fromTheme("tools"), tr("Inspector"));
        check = connect(m_pInspector, &QAction::toggled,
                        this, &CFrmWebBrowser::slotInspector);
        Q_ASSERT(check);
        m_pInspector->setStatusTip(tr("Inspector"));
        m_pInspector->setCheckable(true);
        m_pInspector->setEnabled(false);
        m_pInspector->setShortcuts({
            QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_I),
            QKeySequence(Qt::Key_F12)
        });
    }

    pMenu->addSeparator();
    QMenu* pMenuBookmark = new QMenu(tr("Bookmark"), pMenu);
    pMenuBookmark->setIcon(QIcon::fromTheme("user-bookmarks"));
    pMenu->addMenu(pMenuBookmark);
    pMenuBookmark->addAction(m_pAddBookmark);
    pMenuBookmark->addAction(tr("Manage Bookmark "), this, &CFrmWebBrowser::slotBookmark);
    pMenu->addAction(tr("History"), this, &CFrmWebBrowser::slotHistory);

    pMenu->addSeparator();
    m_pCapturePage = pMenu->addAction(
        QIcon::fromTheme("screen-shot"), tr("Capture page"),
        this, &CFrmWebBrowser::slotCapturePage);
    m_pCapturePage->setStatusTip(tr("Capture page"));
    m_pCaptureFulPage = pMenu->addAction(
        QIcon::fromTheme("screen-shot"), tr("Capture full page"),
        this, &CFrmWebBrowser::slotCaptureFullPage);
    m_pCaptureFulPage->setStatusTip(tr("Capture full page"));
    m_pRecord = pMenu->addAction(QIcon::fromTheme("media-record"), tr("Record"),
                     this, &CFrmWebBrowser::slotRecord);
    m_pRecord->setCheckable(true);
    m_pRecord->setStatusTip(tr("Record"));
#ifdef HAVE_QT6_RECORD
    m_pRecord->setEnabled(true);
#else
    m_pRecord->setEnabled(false);
#endif

    EnableAction(false);
    return 0;
}

int CFrmWebBrowser::Start()
{
    int nRet = 0;
    if(m_pTab && m_pTab->count() == 0) {
        // Add new web view
        m_pAddPage->trigger();
    }
    return nRet;
}

int CFrmWebBrowser::Stop()
{
    int nRet = 0;

    if(m_pRecord && m_pRecord->isChecked())
        slotRecord();
    if(m_pTab) {
        for(int i = 0; i < m_pTab->count(); i++) {
            auto v = GetView(i);
            if(!v) continue;
            v->stop();
        }
    }
    return nRet;
}

void CFrmWebBrowser::slotTabCurrentChanged(int index)
{
    if(-1 == index) return;
    CFrmWebView* pWeb = CurrentView();
    if(pWeb) {
        setWindowTitle(pWeb->title());
        setWindowIcon(pWeb->icon());
        m_pUrlLineEdit->setText(pWeb->url().toString());
        m_pProgressBar->setValue(pWeb->progress());
        EnableAction(true);
        auto page = pWeb->page();
        if(page) {
            auto action = page->action(QWebEnginePage::Back);
            if(action && m_pBack) {
                m_pBack->setEnabled(action->isEnabled());
            }
            action = page->action(QWebEnginePage::Forward);
            if(action && m_pForward) {
                m_pForward->setEnabled(action->isEnabled());
            }
            action = page->action(QWebEnginePage::Reload);
            if(action && m_pRefresh)
                m_pRefresh->setEnabled(action->isEnabled());
            if(m_pRefresh->isEnabled())
                m_pToolBar->insertAction(m_pUrl, m_pRefresh);
            else
                m_pToolBar->removeAction(m_pRefresh);
            action = page->action(QWebEnginePage::Stop);
            if(action && m_pStop)
                m_pStop->setEnabled(action->isEnabled());
            if(m_pStop->isEnabled())
                m_pToolBar->insertAction(m_pUrl, m_pStop);
            else
                m_pToolBar->removeAction(m_pStop);
            if(m_pInspector) {
                m_pInspector->setChecked(CurrentView(ViewType::DevTools));
            }
        }
    } else {
        setWindowTitle(tr("Web browser"));
        setWindowIcon(QIcon::fromTheme("web-browser"));
        m_pUrlLineEdit->setText("");
        m_pProgressBar->setValue(100);
        EnableAction(false);
        if(m_pInspector)
            m_pInspector->setChecked(false);
    }
    emit sigUpdateTitle();
}

void CFrmWebBrowser::EnableAction(bool enable)
{
    if(m_pBack)
        m_pBack->setEnabled(false);
    if(m_pForward)
        m_pForward->setEnabled(false);
    if(m_pRefresh)
        m_pRefresh->setEnabled(enable);
    if(m_pStop)
        m_pStop->setEnabled(false);
    if(m_pFind)
        m_pFind->setEnabled(enable);
    if(m_pFindNext)
        m_pFindNext->setEnabled(enable);
    if(m_pFindPrevious)
        m_pFindPrevious->setEnabled(enable);
    if(m_pZoomOriginal)
        m_pZoomOriginal->setEnabled(enable);
    if(m_pZoomIn)
        m_pZoomIn->setEnabled(enable);
    if(m_pZoomOut)
        m_pZoomOut->setEnabled(enable);
    if(m_pInspector)
        m_pInspector->setEnabled(enable);
    if(m_pRecord)
        m_pRecord->setEnabled(enable);
}

void CFrmWebBrowser::slotTabCloseRequested(int index)
{
    qDebug(log) << "slotTabCloseRequested:" << index;
    if(-1 == index || !m_pTab) return;
    auto pView = m_pTab->widget(index);
    if(pView)
        pView->deleteLater();
    m_pTab->removeTab(index);
}

void CFrmWebBrowser::slotViewCloseRequested()
{
    CFrmWebView* p = qobject_cast<CFrmWebView*>(sender());
    if(!p) return;
    int index = IndexOfTab(p);
    slotTabCloseRequested(index);
}

void CFrmWebBrowser::slotReturnPressed()
{
    slotUrlSelected(m_pUrlLineEdit->text());
}

void CFrmWebBrowser::slotUrlSelected(const QString &szUrl)
{
    if(szUrl.isEmpty()) return;
    QUrl url = QUrl::fromUserInput(szUrl);
    qDebug(log) << url << szUrl << url.isValid();
    if(szUrl.startsWith("@search:", Qt::CaseInsensitive)) {
        QString szSearch;
        QString keyword = szUrl.mid(8);
        if(m_pPara) {
            szSearch = m_pPara->GetSearchEngine();
            url = szSearch.replace(m_pPara->GetSearchRelaceString(),
                                 QUrl::toPercentEncoding(keyword));
        }
    }
    emit sigInformation(url.toString());
    CFrmWebView* pWeb = CurrentView();
    if(!pWeb)
        pWeb = qobject_cast<CFrmWebView*>(CreateWindow(QWebEnginePage::WebBrowserTab));
    pWeb->load(url);
    if(m_pGo->isVisible())
        m_pGo->setVisible(false);
}

void CFrmWebBrowser::slotInspector(bool checked)
{
    CFrmWebView* pWeb = CurrentView();
    if(!pWeb || !m_pTab) return;
    auto dev = CurrentView(ViewType::DevTools);
    if(pWeb && pWeb->page() && checked) {
        if(!dev) {
            auto w = m_pTab->currentWidget();
            if(!w) return;
            QSplitter *pSplitter = qobject_cast<QSplitter*>(w);
            if(!pSplitter || 1 != pSplitter->count()) return;
            dev = CreateWebView(pWeb->page()->profile()->isOffTheRecord());
            if(dev) {
                dev->show();
                pSplitter->addWidget(dev);
                bool check = connect(pWeb, &CFrmWebView::sigDevToolsRequested,
                                     this, [&](){
                                         if(m_pInspector)
                                             m_pInspector->setChecked(true);
                                     });
                Q_ASSERT(check);
                check = connect(dev, &CFrmWebView::sigCloseRequested,
                                this, [this]() {
                    m_pInspector->setChecked(false);
                });
                Q_ASSERT(check);
            }
        }
        if(dev) {
            pWeb->page()->setDevToolsPage(dev->page());
            if(dev->isHidden())
                dev->show();
        }
    } else {
        pWeb->page()->setDevToolsPage(nullptr);
        if(dev) {
            dev->setParent(nullptr);
            dev->deleteLater();
        }
    }
}

int CFrmWebBrowser::Load(QSettings &set)
{
    if(m_pPara && m_pPara->GetOpenPrevious()) {
        set.beginGroup("OpenPrevious");
        int nCount = 0;
        nCount = set.value("Count", 0).toInt();
        int nCurrent = set.value("Current",  -1).toInt();
        for(int i = 0; i < nCount; i++)
        {
            QString u = set.value("Url/" + QString::number(i)).toString();
            auto pView = CreateWindow(QWebEnginePage::WebBrowserTab);
            pView->load(QUrl(u));

            QByteArray history;
            history = set.value("History/" + QString::number(i)).toByteArray();
            QDataStream d(&history, QIODevice::ReadWrite);
            d >> *pView->history();
        }
        if(-1 < nCurrent && m_pTab->count() > nCurrent)
            m_pTab->setCurrentIndex(nCurrent);
        set.endGroup();
    }
    return 0;
}

int CFrmWebBrowser::Save(QSettings &set)
{
    if(m_pPara && m_pPara->GetOpenPrevious()) {
        set.beginGroup("OpenPrevious");
        set.setValue("Count", m_pTab->count());
        set.setValue("Current", m_pTab->currentIndex());

        for(int i = 0; i < m_pTab->count(); i++) {
            auto v = GetView(i);
            if(v) {
                set.setValue("Url/" + QString::number(i), v->url().toString());
                QByteArray history;
                QDataStream d(&history, QIODevice::ReadWrite);
                d << *v->history();
                set.setValue("History/" + QString::number(i), history);
            }
        }

        set.endGroup();
    }
    return 0;
}

void CFrmWebBrowser::slotPrint()
{
    CFrmWebView* pWeb = CurrentView();
    if(pWeb) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
        QPrinter printer;
        pWeb->print(&printer);
#endif
    }
}

void CFrmWebBrowser::slotPrintFinished(bool success)
{
    if(success && m_pPara->GetPromptPrintFinished()) {
        QMessageBox::information(this, tr("Print finished"),
                                 tr("Successfully printed"));
    }
}

void CFrmWebBrowser::slotPrintToPdf()
{
    CFrmWebView* pWeb = CurrentView();
    if(pWeb) {
        QString szPath;
        szPath += RabbitCommon::CDir::Instance()->GetDirUserData()
                  + QDir::separator() + "pdf";
        QDir d(szPath);
        if(!d.exists())
            d.mkpath(szPath);
        szPath += QDir::separator() + pWeb->page()->title() + ".pdf";
        qDebug(log) << "pdf:" << szPath;
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
        pWeb->printToPdf(szPath);
#endif
    }
}

void CFrmWebBrowser::slotPdfPrintingFinished(const QString& szFile, bool success) {
    if(success && m_pPara->GetPromptPrintFinished())
        QMessageBox::information(this, tr("Print to PDF finished"),
                                 tr("Successfully printed to PDF.") + "\n"
                                     + tr("PDF file: ") + szFile);
    qDebug(log) << "Print to PDF:" << szFile << success;
}

void CFrmWebBrowser::slotFullScreen(bool bFullScreen)
{
    if(sender() == this)
        return;
    if(bFullScreen) {
        m_pToolBar->hide();
        m_pProgressBar->hide();
        m_pTab->tabBar()->setVisible(false);
        m_szStyleSheet = m_pTab->styleSheet();
        m_pTab->setStyleSheet("QTabWidget::pane{top:0px;left:0px;border:none;}");
    } else {
        m_pToolBar->show();
        m_pProgressBar->show();
        m_pTab->tabBar()->setVisible(true);
        m_pTab->setStyleSheet(m_szStyleSheet);
        if(CurrentView() && CurrentView()->page() && CurrentView()->page()->action(QWebEnginePage::ExitFullScreen))
            CurrentView()->page()->action(QWebEnginePage::ExitFullScreen)->toggle();
    }
    emit sigFullScreen(bFullScreen);
}

void CFrmWebBrowser::slotCapturePage()
{
    auto pWeb = CurrentView();
    if(!pWeb) return;

    QString szMsg;
    QPixmap pixmap = pWeb->grab();
    QImage image = pixmap.toImage();
    QString szFile = m_pPara->m_Record.GetImageFile(true);
    if(szFile.isEmpty() && image.isNull()) return;
    if(!image.save(szFile, "PNG"))
    {
        QString szErr;
        szErr = tr("Fail: Save capture page to the file: ") + szFile;
        qCritical(log) << szErr;
        emit sigError(RV::FailCapturePage, szErr);
        return;
    }
    szMsg = tr("Save capture page to the file：") + szFile;
    qInfo(log) << szMsg;
    emit sigInformation(szMsg);
    qDebug(log) << "End action:" << m_pPara->m_Record.GetEndAction();
    switch(m_pPara->m_Record.GetEndAction())
    {
    case CParameterRecord::ENDACTION::OpenFile: {
        bool bRet = QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
        if(!bRet)
            qCritical(log) << "Fail: Open capture page the file:" << szFile;
        break;
    }
    case CParameterRecord::ENDACTION::OpenFolder: {
        QFileInfo fi(szFile);
        QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
        break;
    }
    default:
        break;
    }
}

void CFrmWebBrowser::slotCaptureFullPage()
{
    auto pWeb = CurrentView();
    if(!pWeb) return;
    qDebug(log) << "contents size:" << pWeb->page()->contentsSize();
    QString szFile = m_pPara->m_Record.GetImageFile(true);
    CCaptureFullPage *pCapture = new CCaptureFullPage();
    if(szFile.isEmpty() || !pCapture)
        return;
    bool check = connect(pCapture, &CCaptureFullPage::sigFinished,
            this, [this, pWeb, szFile, pCapture](){
        pWeb->setEnabled(true);
        qDebug(log) << "End action:" << m_pPara->m_Record.GetEndAction();
        QFileInfo fi(szFile);
        if(!fi.exists()) {
            QString szErr = tr("Fail: capture full page");
            qCritical(log) << szErr;
            emit sigError(FailCaptureFullPage, szErr);
        } else {
            QString szMsg = tr("Capture full page to") + " " + szFile;
            qInfo(log) << szMsg;
            emit sigInformation(szMsg);
            switch(m_pPara->m_Record.GetEndAction())
            {
            case CParameterRecord::ENDACTION::OpenFile: {
                bool bRet = QDesktopServices::openUrl(QUrl::fromLocalFile(szFile));
                if(!bRet)
                    qCritical(log) << "Fail: Open capture page the file:" << szFile;
                break;
            }
            case CParameterRecord::ENDACTION::OpenFolder: {
                QFileInfo fi(szFile);
                QDesktopServices::openUrl(QUrl::fromLocalFile(fi.absolutePath()));
                break;
            }
            default:
                break;
            }
            pCapture->deleteLater();
        }
    });
    Q_ASSERT(check);
    pWeb->setEnabled(false);
    pCapture->Start(pWeb, pWeb->url(), szFile);

    emit sigInformation(tr("Start capture full page"));
}

void CFrmWebBrowser::slotRecord()
{
    if(m_pRecord->isChecked()) {
        m_pRecord->setText(tr("Stop record"));
        m_pRecord->setIcon(QIcon::fromTheme("media-stop"));

        Q_ASSERT(!m_pMultimediaRecord);
        // Note: It automatically releases memory when exiting.
        // See: CMultimediaRecordThread::CMultimediaRecordThread()
        m_pMultimediaRecord = new CMultimediaRecordThread(m_pPara);
        if(m_pMultimediaRecord) {
            connect(m_pMultimediaRecord, &CMultimediaRecordThread::finished,
                    this, [this]() {
                        m_pMultimediaRecord = nullptr;
                        if(m_pRecord->isChecked()) {
                            m_pRecord->activate(QAction::Trigger);
                            emit sigError(RV::FailRecordPage, tr("Fail record web page"));
                        }
                    });
            m_pMultimediaRecord->start();
        }

        // modify interval
        qreal rate = m_pPara->m_Record.GetVideoFrameRate();
        if(rate <= 0)
            rate = 24;
        m_tmRecord.start(qreal(1000) / rate);
        emit sigInformation(tr("Start record web page"));
    } else {
        m_pRecord->setIcon(QIcon::fromTheme("media-record"));
        m_pRecord->setText(tr("Record"));
        m_tmRecord.stop();
        if(m_pMultimediaRecord) {
            QMetaObject::invokeMethod(m_pMultimediaRecord,
                                      "slotQuit",
                                      Qt::AutoConnection);
            m_pMultimediaRecord = nullptr;
            emit sigInformation(tr("Record web page is stopped"));
        }
    }
    m_pRecord->setStatusTip(m_pRecord->text());
}

void CFrmWebBrowser::slotRecordTimeout()
{
    //qDebug(log) << Q_FUNC_INFO;
    if(!m_pMultimediaRecord) return;
    auto pWeb = CurrentView();
    if(!pWeb) return;
    QPixmap pixmap = pWeb->grab();
    QImage image = pixmap.toImage();
    if(image.isNull()) return;
    QMetaObject::invokeMethod(m_pMultimediaRecord,
                              "slotUpdateVideoFrame",
                              Qt::AutoConnection,
                              Q_ARG(QImage, image));
}

void CFrmWebBrowser::slotAddBookmark()
{
    auto pView = CurrentView();
    if(!pView) return;

    QUrl url = pView->url();
    if(url.isEmpty())
        url = m_pUrlLineEdit->text();
    CFrmAddBookmark* pAdd = new CFrmAddBookmark(
        pView->title(), url, pView->favIcon(),
        m_pPara);
    bool check = connect(this, &CFrmWebBrowser::destroyed, pAdd, &CFrmAddBookmark::close);
    Q_ASSERT(check);
    RC_SHOW_WINDOW(pAdd);
}

void CFrmWebBrowser::slotBookmark()
{
    CFrmBookmark* pBookmark = new CFrmBookmark(m_pPara);
    if(!pBookmark) return;
    pBookmark->setAttribute(Qt::WA_DeleteOnClose);
    connect(this, &CFrmWebBrowser::destroyed, pBookmark, &CFrmBookmark::close);
    connect(pBookmark, &CFrmBookmark::openUrlRequested, this, [&](const QString& url) {
        CFrmWebView* pWeb = CurrentView();
        if(!pWeb) {
            pWeb = qobject_cast<CFrmWebView*>(CreateWindow(QWebEnginePage::WebBrowserTab));
        }
        if(pWeb)
            pWeb->load(url);
    });
    RC_SHOW_WINDOW(pBookmark);
}

void CFrmWebBrowser::slotHistory()
{
    CFrmHistory* pHistory = new CFrmHistory(m_pPara);
    if(!pHistory) return;
    pHistory->setAttribute(Qt::WA_DeleteOnClose);
    connect(this, &CFrmWebBrowser::destroyed, pHistory, &CFrmHistory::close);
    connect(pHistory, &CFrmHistory::sigOpenUrl, this, [&](const QString& url) {
        CFrmWebView* pWeb = CurrentView();
        if(!pWeb) {
            pWeb = qobject_cast<CFrmWebView*>(CreateWindow(QWebEnginePage::WebBrowserTab));
        }
        if(pWeb)
            pWeb->load(url);
    });
    connect(pHistory, &CFrmHistory::sigOpenUrlInNewTab,
            this, [&](const QString& url) {
                auto pWeb = qobject_cast<CFrmWebView*>(CreateWindow(QWebEnginePage::WebBrowserTab));
                if(pWeb)
                    pWeb->load(url);
            });
    RC_SHOW_WINDOW(pHistory);
}
