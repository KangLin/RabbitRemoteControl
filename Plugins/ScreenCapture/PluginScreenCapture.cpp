// Author: Kang Lin <kl222@126.com>

#include "PluginScreenCapture.h"
#include "ScreenCapture.h"

CPluginScreenCapture::CPluginScreenCapture(QObject *parent)
    : CPlugin{parent}
{}

const CPlugin::TYPE CPluginScreenCapture::Type() const
{
    return TYPE::Tools;
}

const QString CPluginScreenCapture::Protocol() const
{
    return QString();
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
    return tr("Capture or record screen or window. This is a sample example.");
}

const QString CPluginScreenCapture::Version() const
{
    return ScreenCapture_VERSION;
}

const QIcon CPluginScreenCapture::Icon() const
{
    return QIcon::fromTheme("camera-photo");
}

COperate *CPluginScreenCapture::OnCreateOperate(const QString &szId)
{
    COperate* p = new CScreenCapture(this);
    return p;
}
