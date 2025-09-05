#include "PluginWebBrowser.h"
#include "OperateWebBrowser.h"

CPluginWebBrowser::CPluginWebBrowser(QObject *parent)
    : CPlugin(parent)
{}

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
    return tr("Web browser");
}

const QString CPluginWebBrowser::Description() const
{
    return tr("Web browser: Browsing the web(Experiment)");
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
    return QString();
}
