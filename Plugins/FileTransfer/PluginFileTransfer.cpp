// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#if HAVE_CURL
    #include <curl/curl.h>
#endif

#include <QApplication>
#include <QStyle>
#include <QLoggingCategory>
#include "PluginFileTransfer.h"
#include "OperateFileTransfer.h"

#ifdef HAVE_LIBSSH
    #include "ChannelSSH.h"
#endif

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
    return tr("File transfer: include sftp, ftp");
}

const QString CPluginFileTransfer::Version() const
{
    return 0;
}

const QIcon CPluginFileTransfer::Icon() const
{
    return QIcon::fromTheme("system-file-manager");
    //return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
}

const QString CPluginFileTransfer::Details() const
{
    QString szDetails;
#ifdef HAVE_LIBSSH
    CChannelSSH channel(nullptr, nullptr);
    szDetails += channel.GetDetails();
#endif

#if HAVE_CURL
    szDetails += "- " + tr("libCurl") + " ";
    curl_version_info_data *version_info = curl_version_info(CURLVERSION_NOW);
    szDetails += tr("Version:") + " " + QString(version_info->version) + "\n";
    szDetails += "  - " + tr("Supported protocols:") + " ";
    if (version_info->protocols) {
        for (int i = 0; version_info->protocols[i]; i++) {
            szDetails += " ";
            szDetails += version_info->protocols[i];
        }
    }
    szDetails += "\n";
    #if CURL_VERSION_SSL
        szDetails += "  - " + QString("SSL supported:") + " " + QString(version_info->features & CURL_VERSION_SSL ? "Yes" : "No") + "\n";
    #endif
    #if CURL_VERSION_LIBSSH2
        szDetails += "  - " + QString("LIBSSH2 supported:") +  " " + QString(version_info->features & CURL_VERSION_LIBSSH2 ? "Yes" : "No") + "\n";
    #endif
#endif
    return szDetails;
}

COperate *CPluginFileTransfer::OnCreateOperate(const QString &szId)
{
    if(Id() == szId)
        return new COperateFileTransfer(this);
    return nullptr;
}
