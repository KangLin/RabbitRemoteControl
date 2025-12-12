// Author: Kang Lin <kl222@126.com>

#include "PluginWebBrowser.h"
#include "OperateWebBrowser.h"
#include <QWebEngineProfile>
#include <QLoggingCategory>

static Q_LOGGING_CATEGORY(log, "WebBrowser.Plugin")
CPluginWebBrowser::CPluginWebBrowser(QObject *parent)
    : CPlugin(parent)
{
    qDebug(log) << Q_FUNC_INFO;
}

CPluginWebBrowser::~CPluginWebBrowser()
{
    qDebug(log) << Q_FUNC_INFO;
}

const CPluginWebBrowser::TYPE CPluginWebBrowser::Type() const
{
    return TYPE::Tools;
}

const QString CPluginWebBrowser::Protocol() const
{
    return QString();
}

const QString CPluginWebBrowser::Name() const
{
    return "WebBrowser";
}

const QString CPluginWebBrowser::DisplayName() const
{
    return tr("Web browser") + tr("(Experimental)");
}

const QString CPluginWebBrowser::Description() const
{
    return tr("Web browser: Browsing the web")  + tr("(Experimental)");
}

const QString CPluginWebBrowser::Version() const
{
    return PluginWebBrowser_VERSION;
}

const QIcon CPluginWebBrowser::Icon() const
{
    return QIcon::fromTheme("web-browser");
}

COperate *CPluginWebBrowser::OnCreateOperate(const QString &szId)
{
    if(Id() == szId)
        return new COperateWebBrowser(this);
    return nullptr;
}

const QString CPluginWebBrowser::Details() const
{
    QString szDetails;
#if QT_VERSION >= QT_VERSION_CHECK(6, 2, 0)
    szDetails += "- QWebEngine" + QString("\n");
    szDetails += "  - " + tr("version:") + " " + qWebEngineVersion() + "\n";
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    szDetails += "  - " + tr("Process name:") + " " + qWebEngineProcessName() + "\n";
#endif
    szDetails += "  - " + tr("Chromium:") + "\n";
    szDetails += "    - " + tr("version:") + " " + qWebEngineChromiumVersion() + "\n";
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
    szDetails += "    - " + tr("Security patch version:") + " " + qWebEngineChromiumSecurityPatchVersion() + "\n";
#endif
    return szDetails;
}
