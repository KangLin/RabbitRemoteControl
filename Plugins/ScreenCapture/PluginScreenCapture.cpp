// Author: Kang Lin <kl222@126.com>

#include "PluginScreenCapture.h"
#include "ScreenCapture.h"

CPluginScreenCapture::CPluginScreenCapture(QObject *parent)
    : CPluginClient{parent}
{}

const CPluginClient::TYPE CPluginScreenCapture::Type() const
{
    return TYPE::Tool;
}

const QString CPluginScreenCapture::Protocol() const
{
    return "Tool";
}

const QString CPluginScreenCapture::Name() const
{
    return "ScreenCapture";
}

const QString CPluginScreenCapture::DisplayName() const
{
    return tr("Capture screen");
}

const QString CPluginScreenCapture::Description() const
{
    return tr("Capture or record screen and window. This is a sample example.");
}

const QString CPluginScreenCapture::Version() const
{
    return ScreenCapture_VERSION;
}

const QIcon CPluginScreenCapture::Icon() const
{
    return QIcon::fromTheme("camera-photo");
}

CConnecter* CPluginScreenCapture::OnCreateConnecter(const QString &szId)
{
    CConnecter* p = new CScreenCapture(this);
    return p;
}
