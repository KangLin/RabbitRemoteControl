// Author: Kang Lin <kl222@126.com>

#include "PluginScreenCapture.h"
#include "ScreenCapture.h"

CPluginScreenCapture::CPluginScreenCapture(QObject *parent)
    : CPluginClient{parent}
{}


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
    return tr("Capture or recored screen and window");
}

const QString CPluginScreenCapture::Version() const
{
    return ScreenCapture_VERSION;
}

const QIcon CPluginScreenCapture::Icon() const
{
    return QIcon::fromTheme("camera-photo");
}

CConnecter *CPluginScreenCapture::CreateConnecter(const QString &szId)
{
    CConnecter* p = new CScreenCapture(this);
    return p;
}