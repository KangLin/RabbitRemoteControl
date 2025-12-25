// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#include <QApplication>
#include <QStyle>
#include <QLoggingCategory>

#include "PluginFtpServer.h"
#include "OperateFtpServer.h"

static Q_LOGGING_CATEGORY(log, "FtpServer.Plugin")

CPluginFtpServer::CPluginFtpServer(QObject *parent)
    : CPlugin{parent}
{
    qDebug(log) << Q_FUNC_INFO;
}

CPluginFtpServer::~CPluginFtpServer()
{
    qDebug(log) << Q_FUNC_INFO;
}

const CPlugin::TYPE CPluginFtpServer::Type() const
{
    return TYPE::Service;
}

const QString CPluginFtpServer::Protocol() const
{
    return "FTP";
}

const QString CPluginFtpServer::Name() const
{
    return "FtpServer";
}

const QString CPluginFtpServer::DisplayName() const
{
    return tr("FTP(File Transfer Protocol) Server");
}

const QString CPluginFtpServer::Description() const
{
    return tr("FTP(File Transfer Protocol) Server: include ftp, ftp-es.") + "\n"
           + tr("It uses: ") + "QFtpServer: https://github.com/KangLin/QFtpServer";
}

const QString CPluginFtpServer::Version() const
{
    return 0;
}

const QIcon CPluginFtpServer::Icon() const
{
    return QIcon::fromTheme("file-transfer");
}

const QString CPluginFtpServer::Details() const
{
    QString szDetails;
    return szDetails;
}

COperate *CPluginFtpServer::OnCreateOperate(const QString &szId)
{
    if(Id() == szId)
        return new COperateFtpServer(this);
    return nullptr;
}
