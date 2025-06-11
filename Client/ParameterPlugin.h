// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Parameter.h"
#include "ParameterClient.h"

/*!
 * \~chinese 插件的公共参数。
 * \details
 * - 仅在管理者 (CManager) 和插件中使用。 
 * - 应用程序不能访问，但是可以通过 CClient::GetSettingsWidgets 进行设置。
 *
 * \~english Common parameters of plugins
 * - The parameters is valid in the CManager and plugin.
 * - The application cannot access it,
 *   but the application can be set it via CClient::GetSettingsWidgets .
 *
 * \~
 * \see COperate::SetParameterPlugin
 * \ingroup CLIENT_PARAMETER
 */
class CLIENT_EXPORT CParameterPlugin : public CParameter
{
    Q_OBJECT
public:
    explicit CParameterPlugin(QObject *parent = nullptr,
                              const QString& szPrefix = QString());

    CParameterClient m_Client;

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
