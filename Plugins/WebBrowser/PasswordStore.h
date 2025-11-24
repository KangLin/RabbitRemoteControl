// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QVariantMap>
#include "ParameterWebBrowser.h"

/*
 CPasswordStore using QtKeychain.

 Notes:
 - Stores credentials in the system keychain using QtKeychain.
 - Credentials are stored as a small JSON object under a single key per host:
     { "username": "...", "password": "..." }
 - getCredentials() blocks briefly waiting for the keychain response (with a timeout).
   This mirrors the previous synchronous QSettings behavior. In a production app you
   may prefer asynchronous APIs or to move keychain access to a worker thread.
 - Requires QtKeychain to be available and linked into the project.
*/
class CPasswordStore : public QObject
{
    Q_OBJECT
public:
    explicit CPasswordStore(CParameterWebBrowser* pPara, QObject *parent = nullptr);
    ~CPasswordStore();
    // From JS: returns { "username": "...", "password":"..." } or empty map
    Q_INVOKABLE QVariantMap getCredentials(const QString &host);
    // From JS: save credentials for host
    Q_INVOKABLE void saveCredentials(const QString &host, const QString &username, const QString &password);
Q_SIGNALS:
    void credentialsSaved(const QString &host);
    void sigCredentialsReady(const QString &host, const QVariantMap& credentials);
private:
    CParameterWebBrowser* m_pPara;
};
