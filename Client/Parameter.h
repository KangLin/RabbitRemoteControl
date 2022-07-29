// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMETER_H_KL_2022_07_27
#define CPARAMETER_H_KL_2022_07_27

#pragma once

#include <QSettings>
#include <QString>
#include "client_export.h"

/*!
 *
 * \~chinese
 * \brief 参数接口类
 * \details
 * 参数有以下类型：
 * 1. 仅在插件内有效。应用程序不能直接访问，
 *    应用程序只能通过 CConnecter::OpenDialogSettings 进行设置。
 *    \see CParameterConnecter
 * 2. 在客户端库内有效。 \see CParameterViwer
 *    应用程序不能直接访问。
 *    应用程序只通过 CManagePlugin::GetSettings 进行设置。
 *    插件可以直接使用或者以其做为初始化值。
 *    例如：保存密码可以以它为初始化值。 \see CParameterConnecter::m_bSavePassword
 * 3. 同时在应用程序和客户端库有效。
 *
 * \~english
 * \brief The CParameter interface class
 *
 * \details
 * The parameter has the following type:
 * 1. Only valid in the plugin.
 *    The application cannot access it directly,
 *    it can only be set via CConnecter::OpenDialogSettings.
 *    \see CParameterConnecter
 * 2. Valid in the Client. \see CParameterViwer
 *    The application cannot access it directly,
 *    it can only be set via CManagePlugin::GetSettings.
 *    Plugins can use them directly or use them as initialization values.
 *    For example: save the password can use it as the initialization value.
 *    \see CParameterConnecter::m_bSavePassword 
 * 3. Valid in both the application and the Client.
 *
 * \~
 * \ingroup VIEWER_PLUGIN_API
 */
class CLIENT_EXPORT CParameter : public QObject
{
    Q_OBJECT

public:
    explicit CParameter(QObject *parent = nullptr);
    virtual ~CParameter();

    virtual int Load(QString szFile = QString());
    virtual int Save(QString szFile = QString());
    virtual int Load(QSettings &set) = 0;
    virtual int Save(QSettings &set) = 0;

Q_SIGNALS:
    void sigChanged();
    // Notify save to file
    void sigSave();
};

#endif // CPARAMETER_H_KL_2022_07_27
