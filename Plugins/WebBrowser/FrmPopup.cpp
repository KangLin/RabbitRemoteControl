
#include <QIcon>
#include <QVBoxLayout>
#include <QWebEnginePage>
#include <QWindow>
#include <QLoggingCategory>
#include "FrmPopup.h"
#include "FrmWebView.h"
#include "FrmWebBrowser.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Widget.Popup")
CFrmPopup::CFrmPopup(QWebEngineProfile *profile, CFrmWebBrowser* pWebBrowser, QWidget *parent)
    : QWidget{parent}
    , m_pleUrl(nullptr)
    , m_pFavAction(nullptr)
    , m_pView(nullptr)
{
    qDebug(log) << Q_FUNC_INFO;
    bool check = false;
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->resize(pWebBrowser->size());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    m_pleUrl = new QLineEdit(this);
    m_pFavAction = new QAction(this);
    layout->addWidget(m_pleUrl);
    m_pView = new CFrmWebView(pWebBrowser, this);
    layout->addWidget(m_pView);
    m_pView->setPage(new QWebEnginePage(profile, m_pView));
    m_pView->setFocus();
    m_pleUrl->setReadOnly(true);
    m_pleUrl->addAction(m_pFavAction, QLineEdit::LeadingPosition);
    check = connect(m_pView, &CFrmWebView::titleChanged, this, &QWidget::setWindowTitle);
    Q_ASSERT(check);
    check = connect(m_pView, &CFrmWebView::urlChanged, [this](const QUrl& url){
        m_pleUrl->setText(url.toDisplayString());
    });
    Q_ASSERT(check);
    check = connect(m_pView, &CFrmWebView::favIconChanged, m_pFavAction, &QAction::setIcon);
    Q_ASSERT(check);
    check = connect(m_pView->page(), &QWebEnginePage::geometryChangeRequested, this, &CFrmPopup::slotHandleGeometryChangeRequested);
    Q_ASSERT(check);
    check = connect(m_pView->page(), &QWebEnginePage::windowCloseRequested, this, &QWidget::close);
    Q_ASSERT(check);
}

CFrmPopup::~CFrmPopup()
{
    qDebug(log) << Q_FUNC_INFO;
}

CFrmWebView* CFrmPopup::GetView()
{
    return m_pView;
}

void CFrmPopup::slotHandleGeometryChangeRequested(const QRect &newGeometry)
{
    if(QWindow* window = windowHandle())
        setGeometry(newGeometry.marginsRemoved(window->frameMargins()));
    show();
    m_pView->setFocus();
}
