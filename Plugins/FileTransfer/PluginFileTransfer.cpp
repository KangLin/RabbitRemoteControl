// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "PluginFileTransfer.h"
#include "OperateFileTransfer.h"

static Q_LOGGING_CATEGORY(log, "Plugin.FileTransfer")

CPluginFileTransfer::CPluginFileTransfer(QObject *parent)
    : CPlugin{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CPluginFileTransfer::~CPluginFileTransfer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const CPlugin::TYPE CPluginFileTransfer::Type() const
{
    return TYPE::FileTransfers;
}

const QString CPluginFileTransfer::Protocol() const
{
    return QString();
}

const QString CPluginFileTransfer::Name() const
{
    return "FileTransfer";
}

const QString CPluginFileTransfer::DisplayName() const
{
    return tr("File transfer");
}

const QString CPluginFileTransfer::Description() const
{
    return tr("File transfer: include sftp, ftp etc");
}

const QString CPluginFileTransfer::Version() const
{
    return 0;
}

const QIcon CPluginFileTransfer::Icon() const
{
    return QIcon::fromTheme("system-file-manager");
}

COperate *CPluginFileTransfer::OnCreateOperate(const QString &szId)
{
    if(Id() == szId)
        return new COperateFileTransfer(this);
    return nullptr;
}
