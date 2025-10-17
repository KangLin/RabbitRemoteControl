// Author: Kang Lin <kl222@126.com>

#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QAuthenticator>
#include <QTimer>
#include <QStyle>
#include <QLoggingCategory>
#include <QWebEngineProfile>

#include "FrmWebView.h"
#include "FrmWebBrowser.h"
#include "DlgUserPassword.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.View")

CFrmWebView::CFrmWebView(CFrmWebBrowser *pFrmWebBrowser, QWidget *parent)
    : QWebEngineView(parent)
    , m_pBrowser(pFrmWebBrowser)
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    , m_pDlgWebAuth(nullptr)
#endif
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(200, 100);
    connect(this, &QWebEngineView::loadStarted, [this]() {
        m_loadProgress = 0;
        emit favIconChanged(favIcon());
    });
    connect(this, &QWebEngineView::loadProgress, [this](int progress) {
        m_loadProgress = progress;
    });
    connect(this, &QWebEngineView::loadFinished, [this](bool success) {
        m_loadProgress = success ? 100 : -1;
        emit favIconChanged(favIcon());
    });
    connect(this, &QWebEngineView::iconChanged, [this](const QIcon &) {
        emit favIconChanged(favIcon());
    });

    connect(this, &QWebEngineView::renderProcessTerminated,
            [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
        QString status;
        switch (termStatus) {
        case QWebEnginePage::NormalTerminationStatus:
            status = tr("Render process normal exit");
            break;
        case QWebEnginePage::AbnormalTerminationStatus:
            status = tr("Render process abnormal exit");
            break;
        case QWebEnginePage::CrashedTerminationStatus:
            status = tr("Render process crashed");
            break;
        case QWebEnginePage::KilledTerminationStatus:
            status = tr("Render process killed");
            break;
        }
        QMessageBox::StandardButton btn = QMessageBox::question(window(), status,
                                                   tr("Render process exited with code: %1\n"
                                                      "Do you want to reload the page ?").arg(statusCode));
        if (btn == QMessageBox::Yes)
            QTimer::singleShot(0, this, &CFrmWebView::reload);
    });
}

CFrmWebView::~CFrmWebView()
{
    qDebug(log) << Q_FUNC_INFO;
    if (m_imageAnimationGroup)
        delete m_imageAnimationGroup;

    m_imageAnimationGroup = nullptr;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
inline QString questionForPermissionType(QWebEnginePermission::PermissionType permissionType)
{
    switch (permissionType) {
    case QWebEnginePermission::PermissionType::Geolocation:
        return QObject::tr("Allow %1 to access your location information?");
    case QWebEnginePermission::PermissionType::MediaAudioCapture:
        return QObject::tr("Allow %1 to access your microphone?");
    case QWebEnginePermission::PermissionType::MediaVideoCapture:
        return QObject::tr("Allow %1 to access your webcam?");
    case QWebEnginePermission::PermissionType::MediaAudioVideoCapture:
        return QObject::tr("Allow %1 to access your microphone and webcam?");
    case QWebEnginePermission::PermissionType::MouseLock:
        return QObject::tr("Allow %1 to lock your mouse cursor?");
    case QWebEnginePermission::PermissionType::DesktopVideoCapture:
        return QObject::tr("Allow %1 to capture video of your desktop?");
    case QWebEnginePermission::PermissionType::DesktopAudioVideoCapture:
        return QObject::tr("Allow %1 to capture audio and video of your desktop?");
    case QWebEnginePermission::PermissionType::Notifications:
        return QObject::tr("Allow %1 to show notification on your desktop?");
    case QWebEnginePermission::PermissionType::ClipboardReadWrite:
        return QObject::tr("Allow %1 to read from and write to the clipboard?");
    case QWebEnginePermission::PermissionType::LocalFontsAccess:
        return QObject::tr("Allow %1 to access fonts stored on this machine?");
    case QWebEnginePermission::PermissionType::Unsupported:
        break;
    }
    return QString();
}
#endif

void CFrmWebView::setPage(QWebEnginePage *page)
{
    CreateWebActionTrigger(page,QWebEnginePage::Forward);
    CreateWebActionTrigger(page,QWebEnginePage::Back);
    CreateWebActionTrigger(page,QWebEnginePage::Reload);
    CreateWebActionTrigger(page,QWebEnginePage::Stop);

    if (auto oldPage = QWebEngineView::page()) {
        oldPage->disconnect(this);
    }
    QWebEngineView::setPage(page);
    connect(page, &QWebEnginePage::linkHovered, this, &CFrmWebView::sigLinkHovered);
    connect(page, &QWebEnginePage::windowCloseRequested,
            this, &CFrmWebView::sigCloseRequested);
    connect(page, &QWebEnginePage::selectClientCertificate,
            this, &CFrmWebView::slotSelectClientCertificate);
    connect(page, &QWebEnginePage::authenticationRequired, this,
            &CFrmWebView::slotAuthenticationRequired);
    connect(page, &QWebEnginePage::proxyAuthenticationRequired, this,
            &CFrmWebView::slotProxyAuthenticationRequired);   
    connect(page, &QWebEnginePage::registerProtocolHandlerRequested, this,
            &CFrmWebView::handleRegisterProtocolHandlerRequested);
    #if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    connect(page, &QWebEnginePage::certificateError,
            this, &CFrmWebView::slotCertificateError);
    connect(page, &QWebEnginePage::permissionRequested, this,
            &CFrmWebView::slotPermissionRequested);
    #endif
    #if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    connect(page, &QWebEnginePage::fileSystemAccessRequested, this,
            &CFrmWebView::slotFileSystemAccessRequested);
    #endif
    #if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    connect(page, &QWebEnginePage::desktopMediaRequested, this,
            &CFrmWebView::slotDesktopMediaRequest);
    connect(page, &QWebEnginePage::webAuthUxRequested,
            this, &CFrmWebView::slotWebAuthUxRequested);
    #endif
}

int CFrmWebView::progress() const
{
    return m_loadProgress;
}

QIcon CFrmWebView::favIcon() const
{
    QIcon favIcon = icon();
    if (!favIcon.isNull())
        return favIcon;

    if (m_loadProgress < 0) {
        static QIcon errorIcon("dialog-error");
        return errorIcon;
    }
    if (m_loadProgress < 100) {
        static QIcon loadingIcon = QIcon::fromTheme("view-refresh");
        return loadingIcon;
    }

    static QIcon defaultIcon("text-html");
    return defaultIcon;
}

QWebEngineView *CFrmWebView::createWindow(QWebEnginePage::WebWindowType type)
{
    if(m_pBrowser)
        return m_pBrowser->CreateWindow(
            type, this->page()->profile()->isOffTheRecord());
    return this;
}

void CFrmWebView::CreateWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction webAction)
{
    QAction *action = page->action(webAction);
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    connect(action, &QAction::enabledChanged, [this, action, webAction](bool enabled){
        qDebug(log) << "webAction:" << webAction << enabled;
        emit sigWebActionEnabledChanged(webAction, enabled);
    });
#else
    connect(action, &QAction::changed, [this, action, webAction]{
        emit sigWebActionEnabledChanged(webAction, action->isEnabled());
    });
#endif
}

void CFrmWebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    menu = createStandardContextMenu();
#else
    menu = page()->createStandardContextMenu();
#endif
    const QList<QAction *> actions = menu->actions();
    auto inspectElement = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::InspectElement));
    if (inspectElement == actions.cend()) {
        auto viewSource = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::ViewSource));
        if (viewSource == actions.cend())
            menu->addSeparator();

        QAction *action = menu->addAction("Open inspector");
        connect(action, &QAction::triggered, [this]() { emit sigDevToolsRequested(page());});
    } else {
        (*inspectElement)->setText(tr("Inspect element"));
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    // add context menu for image policy
    QMenu *editImageAnimation = new QMenu(tr("Image animation policy"));

    m_imageAnimationGroup = new QActionGroup(editImageAnimation);
    m_imageAnimationGroup->setExclusive(true);

    QAction *disableImageAnimation =
            editImageAnimation->addAction(tr("Disable all image animation"));
    disableImageAnimation->setCheckable(true);
    m_imageAnimationGroup->addAction(disableImageAnimation);
    connect(disableImageAnimation, &QAction::triggered, [this]() {
        handleImageAnimationPolicyChange(QWebEngineSettings::ImageAnimationPolicy::Disallow);
    });
    QAction *allowImageAnimationOnce =
            editImageAnimation->addAction(tr("Allow animated images, but only once"));
    allowImageAnimationOnce->setCheckable(true);
    m_imageAnimationGroup->addAction(allowImageAnimationOnce);
    connect(allowImageAnimationOnce, &QAction::triggered,
            [this]() { handleImageAnimationPolicyChange(QWebEngineSettings::ImageAnimationPolicy::AnimateOnce); });
    QAction *allowImageAnimation = editImageAnimation->addAction(tr("Allow all animated images"));
    allowImageAnimation->setCheckable(true);
    m_imageAnimationGroup->addAction(allowImageAnimation);
    connect(allowImageAnimation, &QAction::triggered, [this]() {
        handleImageAnimationPolicyChange(QWebEngineSettings::ImageAnimationPolicy::Allow);
    });

    switch (page()->settings()->imageAnimationPolicy()) {
    case QWebEngineSettings::ImageAnimationPolicy::Allow:
        allowImageAnimation->setChecked(true);
        break;
    case QWebEngineSettings::ImageAnimationPolicy::AnimateOnce:
        allowImageAnimationOnce->setChecked(true);
        break;
    case QWebEngineSettings::ImageAnimationPolicy::Disallow:
        disableImageAnimation->setChecked(true);
        break;
    default:
        allowImageAnimation->setChecked(true);
        break;
    }

    menu->addMenu(editImageAnimation);
#endif
    menu->popup(event->globalPos());
}

void CFrmWebView::slotSelectClientCertificate(QWebEngineClientCertificateSelection clientCertSelection)
{
    qDebug(log) << Q_FUNC_INFO;
    if(clientCertSelection.certificates().size() > 0) {
        // Just select one.
        clientCertSelection.select(clientCertSelection.certificates().at(0));
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
void CFrmWebView::slotCertificateError(QWebEngineCertificateError error)
{
    qDebug(log) << Q_FUNC_INFO;

    // Automatically block certificate errors from page resources without prompting the user.
    // This mirrors the behavior found in other major browsers.
    if (!error.isMainFrame()) {
        error.rejectCertificate();
        return;
    }

    error.defer();
    QString szMsg;
    szMsg = error.description() + "\n\n";
    szMsg += tr("If you wish so, you may continue with an unverified certificate. "
                "Accepting an unverified certificate mean you may not be connected with the host you tried to connect to.") +
                "\n\n" + tr("Do you wish to override the security check and continue ?");
    int nRet = QMessageBox::critical(window(), tr("Certificate Error"), szMsg,
                          QMessageBox::StandardButton::Yes
                              | QMessageBox::StandardButton::No,
                          QMessageBox::StandardButton::No);
    if(QMessageBox::StandardButton::Yes == nRet)
        error.acceptCertificate();
    else
        error.rejectCertificate();
}
#endif

void CFrmWebView::slotAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth)
{
    qDebug(log) << Q_FUNC_INFO;
    CParameterNet net(nullptr);
    net.SetHost(requestUrl.toString().toHtmlEscaped());
    CDlgUserPassword dlg(this);
    dlg.SetContext(&net);
    if (dlg.exec() == QDialog::Accepted) {
        auto &user = net.m_User;
        auth->setUser(user.GetName());
        auth->setPassword(user.GetPassword());
    } else {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
void CFrmWebView::slotPermissionRequested(QWebEnginePermission permission)
{
    qDebug(log) << Q_FUNC_INFO;
    QString title = tr("Permission Request");
    QString question = questionForPermissionType(permission.permissionType()).arg(permission.origin().host());
    if (!question.isEmpty() && QMessageBox::question(window(), title, question) == QMessageBox::Yes)
        permission.grant();
    else
        permission.deny();
}

void CFrmWebView::handleImageAnimationPolicyChange(QWebEngineSettings::ImageAnimationPolicy policy)
{
    qDebug(log) << Q_FUNC_INFO;
    if (!page())
        return;
    
    page()->settings()->setImageAnimationPolicy(policy);
}
#endif

void CFrmWebView::slotProxyAuthenticationRequired(const QUrl &url, QAuthenticator *auth,
                                                const QString &proxyHost)
{
    qDebug(log) << Q_FUNC_INFO;
    CParameterNet net(nullptr);
    net.SetHost(proxyHost.toHtmlEscaped());
    CDlgUserPassword dlg(this);
    dlg.SetContext(&net);
    if (dlg.exec() == QDialog::Accepted) {
        auto &user = net.m_User;
        auth->setUser(user.GetName());
        auth->setPassword(user.GetPassword());
    } else {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
    /*
    QDialog dialog(window());
    dialog.setModal(true);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    Ui::PasswordDialog passwordDialog;
    passwordDialog.setupUi(&dialog);

    passwordDialog.m_iconLabel->setText(QString());
    QIcon icon(window()->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, window()));
    passwordDialog.m_iconLabel->setPixmap(icon.pixmap(32, 32));

    QString introMessage = tr("Connect to proxy \"%1\" using:");
    introMessage = introMessage.arg(proxyHost.toHtmlEscaped());
    passwordDialog.m_infoLabel->setText(introMessage);
    passwordDialog.m_infoLabel->setWordWrap(true);

    if (dialog.exec() == QDialog::Accepted) {
        auth->setUser(passwordDialog.m_userNameLineEdit->text());
        auth->setPassword(passwordDialog.m_passwordLineEdit->text());
    } else {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
    */
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
void CFrmWebView::slotDesktopMediaRequest(const QWebEngineDesktopMediaRequest &request)
{
    qDebug(log) << Q_FUNC_INFO;
    // select the primary screen
    request.selectScreen(request.screensModel()->index(0));
}

void CFrmWebView::slotWebAuthUxRequested(QWebEngineWebAuthUxRequest *request)
{
    qDebug(log) << Q_FUNC_INFO;

    if (m_pDlgWebAuth)
        delete m_pDlgWebAuth;

    m_pDlgWebAuth = new CDlgWebAuth(request, window());
    m_pDlgWebAuth->setModal(false);
    m_pDlgWebAuth->setWindowFlags(m_pDlgWebAuth->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(request, &QWebEngineWebAuthUxRequest::stateChanged, this, &CFrmWebView::onStateChanged);
    m_pDlgWebAuth->show();

}

void CFrmWebView::onStateChanged(QWebEngineWebAuthUxRequest::WebAuthUxState state)
{
    qDebug(log) << Q_FUNC_INFO;

    if (QWebEngineWebAuthUxRequest::WebAuthUxState::Completed == state
        || QWebEngineWebAuthUxRequest::WebAuthUxState::Cancelled == state) {
        if (m_pDlgWebAuth) {
            delete m_pDlgWebAuth;
            m_pDlgWebAuth = nullptr;
        }
    } else {
        m_pDlgWebAuth->updateDisplay();
    }
}
#endif

//! [registerProtocolHandlerRequested]
void CFrmWebView::handleRegisterProtocolHandlerRequested(
        QWebEngineRegisterProtocolHandlerRequest request)
{
    qDebug(log) << Q_FUNC_INFO;
    auto answer = QMessageBox::question(window(), tr("Permission Request"),
                                        tr("Allow %1 to open all %2 links?")
                                                .arg(request.origin().host())
                                                .arg(request.scheme()));
    if (answer == QMessageBox::Yes)
        request.accept();
    else
        request.reject();
}
//! [registerProtocolHandlerRequested]

#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
void CFrmWebView::slotFileSystemAccessRequested(QWebEngineFileSystemAccessRequest request)
{
    qDebug(log) << Q_FUNC_INFO;
    QString accessType;
    switch (request.accessFlags()) {
    case QWebEngineFileSystemAccessRequest::Read:
        accessType = "read";
        break;
    case QWebEngineFileSystemAccessRequest::Write:
        accessType = "write";
        break;
    case QWebEngineFileSystemAccessRequest::Read | QWebEngineFileSystemAccessRequest::Write:
        accessType = "read and write";
        break;
    default:
        Q_UNREACHABLE();
    }

    auto answer = QMessageBox::question(window(), tr("File system access request"),
                                        tr("Give %1 %2 access to %3?")
                                                .arg(request.origin().host())
                                                .arg(accessType)
                                                .arg(request.filePath().toString()));
    if (answer == QMessageBox::Yes)
        request.accept();
    else
        request.reject();
}
#endif
