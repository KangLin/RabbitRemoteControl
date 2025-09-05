#include "FrmWebBrowser.h"
#include <QVBoxLayout>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "WebBrowser.Browser")
CFrmWebBrowser::CFrmWebBrowser(QWidget *parent)
    : QWidget{parent}
{
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
    m_pBack = m_pToolBar->addAction(QIcon::fromTheme("go-previous"), tr("Back"),
                                    this, [&](){
        CFrmWebView* pWeb = qobject_cast<CFrmWebView*>(m_pTab->currentWidget());
        if(pWeb)
            pWeb->page()->action(QWebEnginePage::Back)->trigger();
    });
    m_pBack->setEnabled(false);
    m_pForward = m_pToolBar->addAction(QIcon::fromTheme("go-next"), tr("Forward"),
                                    this, [&](){
        CFrmWebView* pWeb = qobject_cast<CFrmWebView*>(m_pTab->currentWidget());
        if(pWeb)
            pWeb->page()->action(QWebEnginePage::Forward)->trigger();
                                    });
    m_pForward->setEnabled(false);
    m_pRefresh = m_pToolBar->addAction(QIcon::fromTheme("view-refresh"), tr("Refresh"),
                                    this, [&](){
        CFrmWebView* pWeb = qobject_cast<CFrmWebView*>(m_pTab->currentWidget());
        if(pWeb)
            pWeb->page()->action(QWebEnginePage::Reload)->trigger();
                                    });
    m_pUrlLineEdit = new QLineEdit(this);
    m_pFavAction = new QAction(this);
    m_pUrlLineEdit->addAction(m_pFavAction, QLineEdit::LeadingPosition);
    m_pUrlLineEdit->setClearButtonEnabled(true);
    m_pToolBar->addWidget(m_pUrlLineEdit);
    bool check = connect(m_pUrlLineEdit, &QLineEdit::returnPressed,
                         this, [&](){
        QUrl u(m_pUrlLineEdit->text());
        if(u.scheme().isEmpty())
            u.setScheme("https");
        qDebug(log) << "Url:" << u.toString();
        CFrmWebView* pWeb = qobject_cast<CFrmWebView*>(m_pTab->currentWidget());
        if(!pWeb)
            pWeb = qobject_cast<CFrmWebView*>(createWindow(QWebEnginePage::WebBrowserTab));
        pWeb->setUrl(u);
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
        pLayout->addWidget(m_pTab);
        m_pTab->setTabsClosable(true);
        m_pTab->setUsesScrollButtons(true);
        m_pTab->setMovable(true);
        m_pTab->setElideMode(Qt::TextElideMode::ElideRight);
    }
    check = connect(m_pTab, &QTabWidget::currentChanged,
                    this, [&](int index) {
        if(-1 == index) return;
        CFrmWebView* pWeb = qobject_cast<CFrmWebView*>(m_pTab->widget(index));
        if(pWeb) {
            SetConnect(pWeb);
            m_pBack->setEnabled(pWeb->page()->action(QWebEnginePage::Back)->isEnabled());
            bool check = connect(pWeb->page()->action(QWebEnginePage::Back), &QAction::enabledChanged,
                    m_pBack, &QAction::setEnabled);
            Q_ASSERT(check);
            m_pForward->setEnabled(pWeb->page()->action(QWebEnginePage::Forward)->isEnabled());
            check = connect(pWeb->page()->action(QWebEnginePage::Forward), &QAction::enabledChanged,
                            m_pForward, &QAction::setEnabled);
            Q_ASSERT(check);
        } else {
            m_pBack->setEnabled(false);
            m_pForward->setEnabled(false);
        }
    });
    Q_ASSERT(check);
    check = connect(m_pTab, &QTabWidget::tabCloseRequested,
                    this, [&](int index){
        qDebug(log) << "tabCloseRequested:" << index;
        if(-1 == index) return;
        auto pView = m_pTab->widget(index);
        if(pView)
            pView->deleteLater();
        m_pTab->removeTab(index);
    });
}

CFrmWebBrowser::~CFrmWebBrowser()
{
    if(m_pToolBar) {
        m_pToolBar->deleteLater();
        m_pToolBar = nullptr;
    }
}

QWebEngineView* CFrmWebBrowser::createWindow(QWebEnginePage::WebWindowType type)
{
    qDebug(log) << "Create window:" << type;
    CFrmWebView* pView = nullptr;
    switch (type) {
    case QWebEnginePage::WebBrowserTab: {
        pView = new CFrmWebView(this);
        int index = m_pTab->addTab(pView, pView->favIcon(), pView->title());
        if(-1 < index)
            m_pTab->setCurrentIndex(index);
        break;
    }
    case QWebEnginePage::WebBrowserBackgroundTab: {
        pView = new CFrmWebView(this);
        int index = m_pTab->currentIndex();
        m_pTab->addTab(pView, pView->favIcon(), pView->title());
        if(-1 < index)
            m_pTab->setCurrentIndex(index);
        break;
    }
    case QWebEnginePage::WebBrowserWindow:
    case QWebEnginePage::WebDialog: {
        qDebug(log) << "Don't support type:" << type;
    }
    }
    return pView;
}

void CFrmWebBrowser::SetConnect(CFrmWebView* pWeb)
{
    bool check = false;
    if(!pWeb) return;
    check = connect(pWeb, &QWebEngineView::urlChanged,
                    this, [&](const QUrl &url){
                        m_pUrlLineEdit->setText(url.toString());
                    });
    Q_ASSERT(check);
    check = connect(pWeb, &CFrmWebView::titleChanged,
                    this, [&](const QString &title) {
        int index = m_pTab->currentIndex();
        if(-1 < index)
            m_pTab->setTabText(index, title);
    });
    Q_ASSERT(check);
    check = connect(pWeb, &CFrmWebView::favIconChanged,
                    this, [&](const QIcon &icon){
                        m_pFavAction->setIcon(icon);
                        int index = m_pTab->currentIndex();
                        if(-1 < index)
                            m_pTab->setTabIcon(index, icon);
                    });
    Q_ASSERT(check);
    check = connect(pWeb, &CFrmWebView::loadProgress,
                    this, [&](int progress){
                        m_pProgressBar->setValue(progress);
                    });
    m_pProgressBar->setValue(pWeb->progress());
}
