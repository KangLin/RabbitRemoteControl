// Author: Kang Lin <kl222@126.com>

#include <QCoreApplication>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QLoggingCategory>
#include <QSettings>
#if HAVE_QTKEYCHAIN
    #include "keychain.h"
#endif

#include "ParameterPlugin.h"
#include "PasswordStore.h"
#include "RabbitCommonEncrypt.h"
#include "RabbitCommonDir.h"

static constexpr int KEYCHAIN_TIMEOUT_MS = 2000; // timeout for blocking waits
static Q_LOGGING_CATEGORY(log, "WebBrowser.PasswordStore")
CPasswordStore::CPasswordStore(CParameterWebBrowser *pPara, QObject *parent)
    : QObject(parent)
    , m_pPara(pPara)
{
    qDebug(log) << Q_FUNC_INFO;
}

CPasswordStore::~CPasswordStore()
{
    qDebug(log) << Q_FUNC_INFO;
}

static QString serviceName()
{
    // Use application name as the keychain service; fall back to a sensible default.
    return "io.github.KangLin/RabbitRemoteControl";
}

#if HAVE_QTKEYCHAIN
QVariantMap CPasswordStore::getCredentials(const QString &host)
{
    QVariantMap result;
    if (host.isEmpty()) return result;

    const QString key = QStringLiteral("autofill/%1").arg(host);
    auto pJob = new QKeychain::ReadPasswordJob(serviceName());
    bool check = connect(pJob, &QKeychain::ReadPasswordJob::finished,
        this, [this, pJob, host]() {
            if (pJob->error() != QKeychain::NoError) {
                qDebug(log) << "ReadPasswordJob error for" << host << ":"
                            << pJob->errorString();
                return;
            }
            QString text = pJob->textData();
            if (!text.isEmpty()) {
                QVariantMap result;
                // Expect JSON { "username": "...", "password": "..." }
                QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
                if (doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj.contains(QStringLiteral("username")))
                        result["username"] = obj.value(QStringLiteral("username")).toString();
                    if (obj.contains(QStringLiteral("password")))
                        result["password"] = obj.value(QStringLiteral("password")).toString();
                } else {
                    // Fallback: treat stored text as password only
                    result["password"] = text;
                }
                qDebug(log) << "Get credentials for" << host;
                emit this->sigCredentialsReady(host, result);
            }
        });
    Q_ASSERT(check);
    pJob->setKey(key);
    pJob->start();

    return result;
}

void CPasswordStore::saveCredentials(
    const QString &host, const QString &username, const QString &password)
{
    if (host.isEmpty() || username.isEmpty()) return;

    const QString key = QStringLiteral("autofill/%1").arg(host);

    QJsonObject obj;
    obj.insert(QStringLiteral("username"), username);
    obj.insert(QStringLiteral("password"), password);
    const QString payload =
        QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    auto pJob = new QKeychain::WritePasswordJob(serviceName());
    bool check = connect(pJob, &QKeychain::WritePasswordJob::finished,
            [this, host, pJob]() {
                if (pJob->error() != QKeychain::NoError) {
                    qDebug(log) << "Saved credentials error for" << host << ":"
                                << pJob->errorString();
                    return;
                }
                emit this->credentialsSaved(host);
                qDebug(log) << "Saved credentials for" << host;
            });
    Q_ASSERT(check);
    pJob->setKey(key);
    pJob->setTextData(payload);
    pJob->start();
}
#else
QVariantMap CPasswordStore::getCredentials(const QString &host)
{
    QVariantMap result;
    if (host.isEmpty()) return result;

    QString szFile;
    szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
             + QDir::separator() + "WebCredentialsStore.ini";
    QSettings set(szFile, QSettings::IniFormat);
    const QString key = QStringLiteral("autofill/%1").arg(host);
    QString text;
    int nRet = m_pPara->LoadPassword("Password", key, text, set);
    if (!nRet && !text.isEmpty()) {
        // Expect JSON { "username": "...", "password": "..." }
        QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
        if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains(QStringLiteral("username")))
                result["username"] = obj.value(QStringLiteral("username")).toString();
            if (obj.contains(QStringLiteral("password")))
                result["password"] = obj.value(QStringLiteral("password")).toString();
        } else {
            // Fallback: treat stored text as password only
            result["password"] = text;
        }
        emit sigCredentialsReady(host, result);
    } else
        qCritical(log) << "Get credentials fail:" << host;

    return result;
}

void CPasswordStore::saveCredentials(
    const QString &host, const QString &username, const QString &password)
{
    if (host.isEmpty() || username.isEmpty()) return;

    QString szFile;
    szFile = RabbitCommon::CDir::Instance()->GetDirUserData()
             + QDir::separator() + "WebCredentialsStore.ini";
    QSettings set(szFile, QSettings::IniFormat);

    const QString key = QStringLiteral("autofill/%1").arg(host);
    QJsonObject obj;
    obj.insert(QStringLiteral("username"), username);
    obj.insert(QStringLiteral("password"), password);
    const QString payload = QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Compact));
    m_pPara->SavePassword(key, payload, set, m_pPara->GetAutoFillUserAndPassword());
    emit credentialsSaved(host);
}
#endif
