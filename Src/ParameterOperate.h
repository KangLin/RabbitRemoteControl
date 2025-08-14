// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterPlugin.h"

/*!
 * \~chinese
 * \brief 操作参数接口。仅在插件内有效。
 *
 * \note
 *  - 仅在插件中实现和使用
 *  - 应用程序不能访问，只能通过 COperate::OpenDialogSettings 进行设置。
 *  - 插件通过 COperateDesktop::GetParameter()、COperateTerminal::GetParameter()　等访问操作参数
 *  - \ref section_Use_CParameterBase
 *  - 需要访问全局参数 (CParameterPlugin) 的分类，需要从此类派生。
 *    通过调用 CParameterOperate::GetGlobalParameters()
 *    访问全局参数 (CParameterPlugin)
 *
 * \details
 * 设置和得到全局参数。需要操作全局参数的参数应该从此类派生。
 * \section section_Use_CParameterPlugin 使用全局参数(CParameterPlugin)
 * \subsection sub_Set_CParameterPlugin_in_CParameterOperate 为操作参数(CParameterOperate 或其派生类)设置全局参数(CParameterPlugin)
 *  - 如果操作参数 ( CParameterOperate 或其派生类 ) 需要全局参数 (CParameterPlugin) 。
 *    - 请在 COperate 派生类的构造函数中实例化操作参数。例如：
 *      \snippet Plugins/FreeRDP/Client/OperateFreeRDP.h Initialize parameter
 *    - 调用 COperateDesktop::SetParameter 设置参数指针。例如：
 *      \snippet Plugins/FreeRDP/Client/OperateFreeRDP.cpp Set the parameter
 *    - 默认会自动为操作参数设置 CParameterPlugin 。详见: CPlugin::CreateOperate 。
 *      \snippet Src/Plugin.cpp CPlugin CreateOperate
 *    - 如果参数不需要 CParameterPlugin ，
 *      那请在 COperate 派生类重载 COperate::SetGlobalParameters 不设置它。
 * \subsection sub_Use_CParameterPlugin_in_CParameterOperate 在操作参数 ( CParameterOperate 或其派生类 ) 中使用全局参数 (CParameterPlugin)
 *   - 在操作参数 ( CParameterOperate 或其派生类 ) 使用 CParameterOperate::GetGlobalParameters() 来得到全局参数 (CParameterPlugin) 。
 *   - 操作参数 ( CParameterOperate 或其派生类 ) 使用全局参数 (CParameterPlugin) 的值做为其初始值。
 *     请重载 CParameterOperate::slotSetGlobalParameters 并在其中初始化相关的值。\n
 *     例如：是否保存密码以 CParameterPlugin 的成员值做为其初始化值。
 *     \snippet Src/ParameterCompone/ParameterUser.cpp Initialize parameter after set CParameterPlugin
 *
 * \section section_CParameterOperate_CATEGORY_USAGE 操作参数分类使用
 *  因为可能会有很多参数，所以需要按参数类型分类。每个分类可以从此类派生出一个单独的类。\n
 *  因为操作参数都有一些基本的参数（例如网络地址等），所以这些基本参数放入 CParameterBase 。
 *  而每个分类做为它的成员变量。
 *
 *  例如：\n
 *  操作参数包括以下几种类型：
 *  - 基本参数
 *    \code
 *    class CParameterBase : public CParameterOperate
 *    \endcode
 *  - 用户参数
 *    \code
 *    class CParameterUser : public CParameterOperate
 *    \endcode
 *  - 视频参数
 *    \code
 *    class CParameterVideo : public CParameter
 *    \endcode
 *  - 音频参数
 *    \code
 *    class CParameterAudio : public CParameter
 *    \endcode
 *
 *  其中 CParameterBase ， CParameterUser 它需要全局参数，
 *  所以必须从 CParameterOperate 派生。
 *  其它的类型则不需要全局参数的，从 CParameter 派生。
 *
 *  那么操作参数可以是以上类型的集合：
 *
 *  \code
 *  class CParameterBase : public CParameterOperate
 *  {
 *  public:
 *      explicit CParameterBase(CParameterOperate *parent = nullptr);
 *
 *      CParameterUser m_User;
 *      CParameterVideo m_Video;
 *      CParameterAudio m_Audio;
 *  };
 *
 *  CParameterBase::CParameterBase(CParameterOperate *parent = nullptr)
 *   : CParameterOperate(parent),
 *     m_User(this, "Host"),
 *     m_Video(this),
 *     m_Audio(this)
 *  {}
 *  \endcode
 *
 *  \note
 *  - 需要全局参数的操作参数必须从 CParameterOperate 派生，否则可以从 CParameter 派生
 *  - 当成员实例化时
 *    - CParameterUser 必须设置构造函数的参数 parent
 *      为 CParameterOperate 的实例（一般设置为 this）。
 *    - m_Video ， m_Audio 必须设置构造函数的参数 parent
 *      为 CParameter 或其派生类的实例（一般设置为 this）。
 *  - \ref sub_Set_CParameterPlugin_in_CParameterOperate
 *
 * \~english
 * \brief Operational parameter interface. Valid only within the plugin.
 *
 * \note
 *  - The interface only is implemented and used by plugin.
 *  - The application cannot access it directly,
 *    it can only be set via COperate::OpenDialogSettings.
 *  - The plugin can access via COperateDesktop::GetParameter(), COperateTerminal::GetParameter() etc
 *  - Need access the global parameters (CParameterPlugin),
 *    must be derived from the class.
 *    Access the global parameters (CParameterPlugin)
 *    via CParameterOperate::GetGlobalParameters()
 *  - \ref section_Use_CParameterBase
 *
 * \details
 * Set and get global parameters. Parameters that need to manipulate global parameters should be derived from this class.
 * \section section_Use_CParameterPlugin Use CParameterPlugin
 * \subsection sub_Set_CParameterPlugin_in_CParameterOperate Set CParameterPlugin for CParameterOperate
 *  - If the parameters of operate(CParameterOperate or it's derived class)
 *    requires a CParameterPlugin.
 *    - Please instantiate the parameters of operate
 *      in the constructor of the COperate or it's derived class. ag:
 *      \snippet Plugins/FreeRDP/Client/OperateFreeRDP.h Initialize parameter
 *    - Call COperateDesktop::SetParameter to set the parameters. ag:
 *      \snippet Plugins/FreeRDP/Client/OperateFreeRDP.cpp Set the parameter
 *    - Default set the CParameterPlugin for the parameters of operate.
 *      See: CPlugin::CreateOperate .
 *      \snippet Src/Plugin.cpp CPlugin CreateOperate
 *    - If you are sure to the parameter does not need CParameterPlugin.
 *      please overload the COperate::SetGlobalParameters
 *      in the COperate derived class, don't set it.
 * \subsection sub_Use_CParameterPlugin_in_CParameterOperate Use CParameterPlugin in CParameterOperate
 *  - Use CParameterOperate::GetGlobalParameters() get CParameterPlugin
 *    in the parameters of operate(CParameterOperate or it's derived class)
 *  - The parameters of operate(CParameterOperate or it's derived class)
 *    use the value of CParameterPlugin as its initial value. \n
 *    Please override CParameterOperate::slotSetGlobalParameters
 *    and initialize the relevant values in it.\n
 *    For example, Whether to save the password
 *    with the member value of CParameterPlugin as its initialization value.
 *     \snippet Src/ParameterCompone/ParameterUser.cpp Initialize parameter after set CParameterPlugin
 *
 * \section section_CParameterOperate_CATEGORY_USAGE CParameterOperate category
 *  Because there may be many parameters,
 *  it is necessary to classify them by parameter category.
 *  Each category can derive a separate class from this class.
 *  Each category is then used as a member variable in the derived classes of this class.
 *
 *  For example:\n
 *  Operate parameters include the following categories:
 *  - Base
 *    \code
 *    class CParameterBase : public CParameterOperate
 *    \endcode
 *  - User
 *    \code
 *    class CParameterUser : public CParameterOperate
 *    \endcode
 *  - Video
 *    \code
 *    class CParameterVideo : public CParameter
 *    \endcode
 *  - Audio
 *    \code
 *    class CParameterAudio : public CParameter
 *    \endcode
 *
 *  CParameterBase, CParameterUser need global parameters,
 *  so that it must derived from CParameterOperate.
 *  other is derived from CParameter.
 *  
 *  Then the operate parameters can be a combination of the above categories:
 *
 *  \code
 *  class CParameterBase : public CParameterOperate
 *  {
 *  public:
 *      explicit CParameterBase(CParameterOperate *parent = nullptr);
 *
 *      CParameterUser m_User;
 *      CParameterVideo m_Video;
 *      CParameterAudio m_Audio;
 *  };
 *
 *  CParameterBase::CParameterBase(CParameterOperate *parent = nullptr)
 *   : CParameterOperate(parent),
 *     m_User(this, "Host"),
 *     m_Video(this),
 *     m_Audio(this)
 *  {}
 *  \endcode
 *
 *  \note
 *  - The connect parameter must be derived from CParameterOperate
 *  - The operate parameters that require global parameters
 *    must be derived from CParameterOperate,
 *    otherwise they can be derived from CParameter
 *  - When a member is instantiated,
 *   the constructor must set the parent to the instance of CParameterOperate(this).
 *  - \ref sub_Set_CParameterPlugin_in_CParameterOperate
 *
 * \~
 * \see CParameterOperate
 *      CParameterPlugin
 *      CManager::CreateOperate
 *      CPlugin::CreateOperate 
 *      COperate::SetGlobalParameters
 *      COperateDesktop::SetParameter
 *      CParameterOperate::GetGlobalParameters
 * \ingroup CLIENT_PARAMETER
 */
class PLUGIN_EXPORT CParameterOperate : public CParameter
{
    Q_OBJECT

public:
    explicit CParameterOperate(
        QObject *parent = nullptr,
        const QString& szPrefix = QString());

    //! Get CParameterPlugin
    CParameterPlugin* GetGlobalParameters();
    int SetGlobalParameters(CParameterPlugin *p);

    const QString GetName() const;
    void SetName(const QString& szName);

private:
Q_SIGNALS:
    /*!
     * \~chinese 仅由此类使用
     * \~english Only used by this class
     * \~
     * \see slotSetGlobalParameters
     */
    void sigSetGlobalParameters();
    void sigNameChanged(const QString &name = QString());

protected Q_SLOTS:
    /*!
     * \~chinese 设置 CParameterPlugin 后调用，用于初始化与 CParameterPlugin 有关的操作。
     *
     * \~english Call after set CParameterPlugin.
     * Used to initialize operations related to CParameterPlugin.
     *
     * \~
     * \ref sub_Use_CParameterPlugin_in_CParameterOperate
     */
    virtual void slotSetGlobalParameters();

protected:
    QByteArray PasswordSum(const std::string &password, const std::string &key);
    int LoadPassword(const QString &szTitle, const QString &szKey,
                     QString &password, QSettings &set);
    int SavePassword(const QString &szKey, const QString &password,
                     QSettings &set, bool bSave = false);

private:
    CParameterOperate* m_Parent;
    /*!
     * \see CManager::CreateConnecter COperate::SetGlobalParameters
     */
    CParameterPlugin* m_pParameterPlugin;

    QString m_szName;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
};
