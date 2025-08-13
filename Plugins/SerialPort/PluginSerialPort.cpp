#include <QLoggingCategory>
#include "PluginSerialPort.h"
#include "OperateSerialPort.h"

static Q_LOGGING_CATEGORY(log, "SerialPort.Plugin")

CPluginSerialPort::CPluginSerialPort(QObject *parent) : CPlugin(parent)
{}

CPluginSerialPort::~CPluginSerialPort()
{}

const QString CPluginSerialPort::Protocol() const
{
    return QString();
}

const QString CPluginSerialPort::Name() const
{
    return "SerialPort";
}

const QString CPluginSerialPort::DisplayName() const
{
    return tr("Serial port terminal");
}

const QString CPluginSerialPort::Description() const
{
    return tr("Serial port terminal");
}

const QIcon CPluginSerialPort::Icon() const
{
    return QIcon::fromTheme("serial-port");
}

const CPlugin::TYPE CPluginSerialPort::Type() const
{
    return CPlugin::TYPE::Terminal;
}

const QString CPluginSerialPort::Version() const
{
    return PluginSerialPort_VERSION;
}

COperate* CPluginSerialPort::OnCreateOperate(const QString &szId)
{
    return new COperateSerialPort(this);
}
