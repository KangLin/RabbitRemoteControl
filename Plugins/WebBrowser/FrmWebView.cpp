// Author: Kang Lin <kl222@126.com>

#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QAuthenticator>
#include <QTimer>
#include <QStyle>
#include <QLoggingCategory>
#include <QWebEngineProfile>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>

#include "RabbitCommonDir.h"

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
    , m_pWebChannel(nullptr)
    , m_pPasswordStore(nullptr)
{
    bool check = false;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMinimumSize(200, 100);
    Q_ASSERT(m_pBrowser);
    check = connect(this, &QWebEngineView::loadStarted, [this]() {
        m_loadProgress = 0;
        emit favIconChanged(favIcon());
    });
    Q_ASSERT(check);
    check = connect(this, &QWebEngineView::loadProgress, [this](int progress) {
        m_loadProgress = progress;
    });
    Q_ASSERT(check);
    check = connect(this, &QWebEngineView::loadFinished, [this](bool success) {
        m_loadProgress = success ? 100 : -1;
        emit favIconChanged(favIcon());
        if(m_pBrowser->m_pPara->GetAutoFillUserAndPassword() && m_pWebChannel && m_pPasswordStore)
            InjectScriptAutoFill();
        return;
        QString szUser, szPassword;
        int nRet = GetUserAndPassword(this->url(), szUser, szPassword);
        if(0 == nRet) {
            SetupAutoFillScript();
            GlobalFillForm(szUser, szPassword);
            //FillForm(szUser, szPassword);
        }
    });
    Q_ASSERT(check);
    check = connect(this, &QWebEngineView::iconChanged, [this](const QIcon &) {
        emit favIconChanged(favIcon());
    });
    Q_ASSERT(check);
    check = connect(this, &QWebEngineView::renderProcessTerminated,
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
    Q_ASSERT(check);
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
    Q_ASSERT(page);
    CreateWebActionTrigger(page, QWebEnginePage::Forward);
    CreateWebActionTrigger(page, QWebEnginePage::Back);
    CreateWebActionTrigger(page, QWebEnginePage::Reload);
    CreateWebActionTrigger(page, QWebEnginePage::Stop);

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
    connect(page, &QWebEnginePage::fullScreenRequested, this,
            &CFrmWebView::slotFullScreenRequested);
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

    Q_ASSERT(m_pBrowser);
    if(m_pBrowser->m_pPara->GetAutoFillUserAndPassword()) {
        if(!m_pWebChannel)
            m_pWebChannel = new QWebChannel(this);
        if(!m_pPasswordStore)
            m_pPasswordStore = new CPasswordStore(m_pBrowser->m_pPara, this);
        if(m_pWebChannel && m_pPasswordStore) {
            // 创建并注册 QWebChannel 对象，暴露 passwordStore 给 JS
            m_pWebChannel->registerObject(QStringLiteral("PasswordManager"), m_pPasswordStore);
            page->setWebChannel(m_pWebChannel);
            InjectScriptQWebChannel();
        } else
            qCritical(log) << "Don't register PasswordManager";
    }
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
        QAction *action = menu->addAction(tr("Open inspector"));
        connect(action, &QAction::triggered,
                [this]() { emit sigDevToolsRequested(page());});
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
    qDebug(log) << Q_FUNC_INFO << error.type() << error.url();

    // Automatically block certificate errors from page resources without prompting the user.
    // This mirrors the behavior found in other major browsers.
    if (!error.isMainFrame()) {
        error.rejectCertificate();
        return;
    }

    error.defer();
    QSettings set(RabbitCommon::CDir::Instance()->GetDirUserData()
                      + QDir::separator() + "WebBrowser.ini",
                  QSettings::IniFormat);
    if(set.value("Certificate/Error/Accept" + error.url().toString(), false).toBool()) {
        error.acceptCertificate();
        return;
    }
    QString szMsg;
    szMsg = error.description() + "\n\n";
    szMsg += tr("If you wish so, you may continue with an unverified certificate. "
                "Accepting an unverified certificate mean you may not be connected with the host you tried to connect to.") + "\n\n"
             + tr("Yes - Always accept an unverified certificate and continue") + "\n"
             + tr("Ok - Accept an unverified certificate and continue only this time") + "\n"
             + tr("Cancel - Reject an unverified certificate and break");
    int nRet = QMessageBox::critical(window(), tr("Certificate Error"), szMsg,
                                     QMessageBox::StandardButton::Ok
                                         | QMessageBox::StandardButton::Yes
                                         | QMessageBox::StandardButton::Cancel,
                                     QMessageBox::StandardButton::Cancel);
    switch(nRet) {
    case QMessageBox::StandardButton::Yes:
        set.setValue("Certificate/Error/Accept" + error.url().toString(), true);
    case QMessageBox::StandardButton::Ok:
        error.acceptCertificate();
        break;
    case QMessageBox::Cancel:
        error.rejectCertificate();
        break;
    }
}
#endif

// Test example:
// - https://postman-echo.com/basic-auth
// - https://httpbin.org/basic-auth/user/passwd
//   - httpbin.org 提供基本的 HTTP 认证测试
//   - 用户名: user, 密码: passwd
void CFrmWebView::slotAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth)
{
    qDebug(log) << Q_FUNC_INFO;
    CParameterUser user(nullptr);
    CDlgUserPassword dlg(this);
    dlg.SetUser(tr("Set user and password") + "\n" + requestUrl.toString(), &user);
    if (dlg.exec() == QDialog::Accepted) {
        auth->setUser(user.GetName());
        auth->setPassword(user.GetPassword());
    } else {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
}

void CFrmWebView::slotProxyAuthenticationRequired(const QUrl &url, QAuthenticator *auth,
                                                  const QString &proxyHost)
{
    qDebug(log) << Q_FUNC_INFO;
    CParameterUser user(nullptr);
    CDlgUserPassword dlg(this);
    dlg.SetUser(tr("Set user and password of proxy") + "\n" + proxyHost.toHtmlEscaped(), &user);
    if (dlg.exec() == QDialog::Accepted) {
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
// Test example: https://thetbw.github.io/web-screen-recorder-demo/
#include "DlgScreenCapture.h"
void CFrmWebView::slotDesktopMediaRequest(const QWebEngineDesktopMediaRequest &request)
{
    qDebug(log) << Q_FUNC_INFO;
    QString szMsg = "- Screen:\n";
    for(int i = 0; i < request.screensModel()->rowCount(); i++) {
        QModelIndex index;
        auto model = request.screensModel();
        index = model->index(i, 0);
        szMsg += "  - " + QString::number(i) + ": " + model->data(index).toString() + "\n";
    }
    szMsg += "- Windows:\n";
    for(int w = 0; w < request.windowsModel()->rowCount(); w++) {
        QModelIndex index;
        auto model = request.windowsModel();
        index = model->index(w, 0);
        szMsg += "  - " + QString::number(w) + ": " + model->data(index).toString() + "\n";
    }
    qDebug(log) << szMsg;

    if(request.screensModel()->rowCount() <= 0 && request.windowsModel()->rowCount() <= 0)
        return;

    CDlgScreenCapture dlg(request);
    if(QDialog::Rejected == dlg.exec())
        return;
    // select the primary screen
    CDlgScreenCapture::Type type;
    int id = -1;
    int nRet = dlg.GetIndex(type, id);
    if(nRet) return;
    QModelIndex index;
    QAbstractListModel* model = nullptr;
    switch(type) {
    case CDlgScreenCapture::Type::Screen: {
        model = request.screensModel();
        if(!model) return;
        if(0 > id || model->rowCount() <= id)
            return;
        index = model->index(id);
        request.selectScreen(index);
        break;
    }
    case CDlgScreenCapture::Type::Window: {
        model = request.windowsModel();
        if(!model) return;
        if(0 > id || model->rowCount() <= id)
            return;
        index = model->index(id);
        request.selectWindow(index);
        break;
    }
    }

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

// Test example: https://www.webmfiles.org/demo-files/
void CFrmWebView::slotFullScreenRequested(QWebEngineFullScreenRequest request)
{
    qDebug(log) << "slotFullScreenRequested";
    if (request.toggleOn()) {
        request.accept();
        this->showFullScreen();
    } else {
        request.accept();
        this->showNormal();
    }
    m_pBrowser->slotFullScreen(request.toggleOn());
}

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

QString CFrmWebView::AutoFillForm()
{
    return R"(
                // 自动填充用户名和密码字段
                function autoFillForm(username, password) {
                    // 查找用户名输入框
                    var usernameFields = [
                        'input[type="text"]',
                        'input[type="email"]',
                        'input[name="username"]',
                        'input[name="user"]',
                        'input[name="email"]',
                        'input[id="username"]',
                        'input[id="user"]',
                        'input[id="email"]'
                    ];

                    // 查找密码输入框
                    var passwordFields = [
                        'input[type="password"]',
                        'input[name="password"]',
                        'input[name="pass"]',
                        'input[id="password"]',
                        'input[id="pass"]'
                    ];

                    var arrUser = [];
                    var arrPassword = [];

                    // 尝试填充用户名
                    usernameFields.forEach(function(selector) {
                        var field = document.querySelector(selector);
                        if (field && !field.value) {
                            arrUser.push(field);
                            // 触发 change 事件
                            //var event = new Event('input', { bubbles: true });
                            //field.dispatchEvent(event);
                        }
                    });

                    // 尝试填充密码
                    passwordFields.forEach(function(selector) {
                        var field = document.querySelector(selector);
                        if (field && !field.value) {
                            arrPassword.push(field);
                            // 触发 change 事件
                            //var event = new Event('input', { bubbles: true });
                            //field.dispatchEvent(event);
                        }
                    });

                    arrUser.forEach(function(input) {
                        if(!input.disabled && !input.readOnly)
                            input.value = username;
                    });
                    arrPassword.forEach(function(input) {
                        if(!input.disabled && !input.readOnly)
                            input.value = password;
                    });
                }
        )";
}

void CFrmWebView::SetupAutoFillScript() {
    // 创建自动填充的 JavaScript
    QString autoFillScript = "(function() {";
    autoFillScript += AutoFillForm();
    autoFillScript += R"(
                // 暴露函数到全局作用域
                window.autoFillForm = autoFillForm;
            })();
        )";
    
    QWebEngineScript script;
    script.setSourceCode(autoFillScript);
    script.setInjectionPoint(QWebEngineScript::DocumentCreation);
    script.setWorldId(QWebEngineScript::MainWorld);
    page()->scripts().insert(script);
}

void CFrmWebView::GlobalFillForm(const QString &szUse, const QString &szPassword)
{
    QString js = QString("window.autoFillForm('%1', '%2');").arg(szUse, szPassword);
    page()->runJavaScript(js);
}

void CFrmWebView::FillForm(const QString &szUse, const QString &szPassword)
{
    QString js = AutoFillForm();
    js += QString("autoFillForm('%1', '%2');").arg(szUse, szPassword);
    page()->runJavaScript(js);
}

int CFrmWebView::GetUserAndPassword(QUrl url, QString &szUser, QString &szPassword)
{
    int nRet = 0;
    //TODO:
    szUser = "aa2";
    szPassword = "p12";
    return nRet;
}

void CFrmWebView::InjectScriptQWebChannel()
{
    Q_ASSERT(page());

    // 从资源载入 qwebchannel.js 和 autofill.js
    // 你需要在 resources.qrc 中把 js/qwebchannel.js 和 js/autofill.js 注册为资源
    auto loadResource = [](const QString &rcPath)->QString{
        QFile f(rcPath);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical(log) << "Cannot open resource" << rcPath;
            return QString();
        }
        return QString::fromUtf8(f.readAll());
    };

    QString qwebchannelJs = loadResource(":/js/QWebChannel.js");
    if (qwebchannelJs.isEmpty()) {
        qCritical(log) << "QWebChannel.js not found in resources!";
        return;
    }
    
    // 先注入 QWebChannel.js，再注入 AutoFill 脚本。将两者合并到一个 QWebEngineScript 也可以。
    QWebEngineScript channelScript;
    channelScript.setName("qwebchannel.js");
    channelScript.setInjectionPoint(QWebEngineScript::DocumentCreation);
    channelScript.setRunsOnSubFrames(true);
    channelScript.setWorldId(QWebEngineScript::MainWorld);
    channelScript.setSourceCode(qwebchannelJs);
    page()->scripts().insert(channelScript);
}

void CFrmWebView::InjectScriptAutoFill()
{
    Q_ASSERT(page());

    // 从资源载入 qwebchannel.js 和 autofill.js
    // 你需要在 resources.qrc 中把 js/qwebchannel.js 和 js/autofill.js 注册为资源
    auto loadResource = [](const QString &rcPath)->QString{
        QFile f(rcPath);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qCritical(log) << "Cannot open resource" << rcPath;
            return QString();
        }
        return QString::fromUtf8(f.readAll());
    };

    QString autofillJs = loadResource(":/js/AutoFill.js");
    if (autofillJs.isEmpty()) {
        qCritical(log) << "AutoFill.js not found in resources!";
        return;
    }
    // QWebEngineScript autoScript;
    // autoScript.setName("autofill.js");
    // autoScript.setInjectionPoint(QWebEngineScript::DocumentReady);
    // autoScript.setRunsOnSubFrames(true);
    // autoScript.setWorldId(QWebEngineScript::MainWorld);
    // autoScript.setSourceCode(autofillJs);
    // page()->scripts().insert(autoScript);

    page()->runJavaScript(autofillJs);
    //qDebug(log) << "autofillJs:" << autofillJs;
    // page()->runJavaScript("typeof qt !== 'undefined' ? 'qt-ok' : 'no-qt'",
    //                       [](const QVariant &v){ qCritical(log) << v.toString(); });
    // page()->runJavaScript("typeof qt !== 'undefined' && typeof qt.webChannelTransport !== 'undefined' ? 'transport-ok' : 'no-transport'",
    //                       [](const QVariant &v){ qCritical(log) << v.toString(); });
}
