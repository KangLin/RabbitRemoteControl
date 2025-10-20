// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QIcon>
#include <QWebEngineView>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineCertificateError>
#include <QWebEngineRegisterProtocolHandlerRequest>
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
#include <QWebEngineFileSystemAccessRequest>
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
#include <QWebEngineWebAuthUxRequest>
#include <QWebEngineDesktopMediaRequest>
#include "DlgWebAuth.h"
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
#include <QWebEnginePermission>
#endif
#include <QActionGroup>
#include <QtWebChannel>

#include "PasswordStore.h"

class CFrmWebBrowser;
class CFrmWebView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit CFrmWebView(CFrmWebBrowser* pFrmWebBrowser, QWidget* parent = nullptr);
    virtual ~CFrmWebView();
    void setPage(QWebEnginePage *page);

    [[nodiscard]] int progress() const;
    [[nodiscard]] QIcon favIcon() const;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;
    void CreateWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction);

signals:
    void favIconChanged(const QIcon &icon);
    void sigDevToolsRequested(QWebEnginePage *source);
    void sigCloseRequested();
    void sigLinkHovered(const QString &url);
    void sigWebActionEnabledChanged(QWebEnginePage::WebAction webAction, bool enabled);

private:
    void SetupAutoFillScript();
    void GlobalFillForm(const QString &username, const QString &szPassword);
    void FillForm(const QString &szUse, const QString &szPassword);
    QString AutoFillForm();
    int GetUserAndPassword(QUrl url, QString &szUser, QString &szPassword);
    void InjectScriptQWebChannel();
    void InjectScriptAutoFill();

private slots:
    void slotSelectClientCertificate(QWebEngineClientCertificateSelection clientCertSelection);
    void slotAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth);
    void slotProxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth,
                                         const QString &proxyHost);
    void handleRegisterProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request);
    
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    void slotCertificateError(QWebEngineCertificateError error);
    void slotPermissionRequested(QWebEnginePermission permission);
    void handleImageAnimationPolicyChange(QWebEngineSettings::ImageAnimationPolicy policy);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    void slotFileSystemAccessRequested(QWebEngineFileSystemAccessRequest request);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    void slotDesktopMediaRequest(const QWebEngineDesktopMediaRequest &request);
    void slotWebAuthUxRequested(QWebEngineWebAuthUxRequest *request);
    void onStateChanged(QWebEngineWebAuthUxRequest::WebAuthUxState state);
#endif

private:
    CFrmWebBrowser* m_pBrowser;
    int m_loadProgress = 100;
    //WebAuthDialog *m_authDialog = nullptr;
    QActionGroup *m_imageAnimationGroup = nullptr;
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    CDlgWebAuth* m_pDlgWebAuth;
#endif
    QWebChannel* m_pWebChannel;
    CPasswordStore* m_pPasswordStore;
};
