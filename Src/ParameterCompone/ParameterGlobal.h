// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Parameter.h"

#include "ParameterDatabase.h"

/*!
 * \~chinese
 * \brief 全局参数
 * \details
 * - 可以在插件管理者 (CManager) 和插件中以及应用中使用。
 * - 应用程序可以通过 CManager::GetGlobalParameters() 得到它。
 * - 插件可以通 CParameterPlugin::GetGlobalParameters() 访问。
 * - 应用通过  CManager::GetSettingsWidgets() 进行设置。
 *
 * \~english
 * \brief Global parameters
 * \details
 * - The parameters is valid in the CManager, plugin and application
 * - The applicat can be accessed via CManager::GetGlobalParameters()
 * - The plugin can be accessed via CParameterPlugin::GetGlobalParameters()
 * - The application can be set it via CManager::GetSettingsWidgets()
 *
 * \~
 * \see CManager::GetGlobalParameters CParameterGlobal
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class PLUGIN_EXPORT CParameterGlobal : public CParameter
{
    Q_OBJECT
public:
    explicit CParameterGlobal(QObject *parent = nullptr,
                              const QString& szPrefix = QString());
    ~CParameterGlobal();

    CParameterDatabase m_Database;

public:
    enum SaveSettingsType{
        File,
        Database
    };
    SaveSettingsType GetSaveSettingsType() const;
    void SetSaveSettingsType(const SaveSettingsType& type);
private:
    SaveSettingsType m_SaveSettingsType;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
