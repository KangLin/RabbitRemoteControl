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
 * 2. 在客户端库和插件内有效。
 *    应用程序不能直接访问。
 *    应用程序只通过 CClient::GetSettingsWidgets 进行设置。
 *    插件可以直接使用或者以其做为初始化值。
 *    例如：保存密码可以以它为初始化值。 详见： CConnecter::SetParameterClient
 *    \see CParameterClient
 * 3. 同时在应用程序、客户端库和插件内有效。暂时没有使用此类型。
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
 * 2. Valid in the Client and the plugin. 
 *    The application cannot access it directly,
 *    it can only be set via CClient::GetSettingsWidgets.
 *    Plugins can use them directly or use them as initialization values.
 *    For example: save the password can use it as the initialization value.
 *    See: CConnecter::SetParameterClient
 *    \see CParameterClient
 * 3. Valid in both the application and the Client and the plugin.
 *    This type is not used at this time.
 *
 * \~
 * \ingroup CLIENT_PLUGIN_API
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

    bool GetModified();

Q_SIGNALS:
    //! Notify changed
    void sigChanged();

protected:
    /*!
     * \brief Must call it when set parameters
     * \param bModified
     */
    int SetModified(bool bModified = true);    
private:
    bool m_bModified;
};

#endif // CPARAMETER_H_KL_2022_07_27
