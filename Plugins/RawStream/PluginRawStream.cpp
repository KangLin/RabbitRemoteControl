#include <QLoggingCategory>
#include "PluginRawStream.h"
#include "OperateRawStream.h"

static Q_LOGGING_CATEGORY(log, "RawStream.Plugin")

CPluginRawStream::CPluginRawStream(QObject *parent) : CPlugin(parent)
{}

CPluginRawStream::~CPluginRawStream()
{}

const QString CPluginRawStream::Protocol() const
{
    return "Raw stream";
}

const QString CPluginRawStream::Name() const
{
    return "RawStream";
}

const QString CPluginRawStream::DisplayName() const
{
    return tr("Terminal") + " - " + tr("Raw stream");
}

const QString CPluginRawStream::Description() const
{
    return tr("Terminal") + " - " + tr("Raw stream");
}

const QIcon CPluginRawStream::Icon() const
{
    return QIcon::fromTheme("console");
}

const CPlugin::TYPE CPluginRawStream::Type() const
{
    return CPlugin::TYPE::Terminal;
}

const QString CPluginRawStream::Version() const
{
    return PluginRawStream_VERSION;
}

COperate* CPluginRawStream::OnCreateOperate(const QString &szId)
{
    return new COperateRawStream(this);
}
