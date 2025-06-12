// Author: Kang Lin <kl222@126.com>

#ifndef CparameterCONNECT_H
#define CparameterCONNECT_H

#pragma once

#include "ParameterClient.h"

/*!
 * \~chinese
 * \defgroup CLIENT_PARAMETER_COMPONE 参数组件
 * \brief 参数组件。
 * - 写一个参数组件：
 *   - 定义参数组件类。例如： CParameterUser 。
 *   - 增加参数组件界面，从 CParameterUI 派生。例如： CParameterUserUI 。 并重载下面函数：
 *     - int SetParameter(CParameter* pParameter);
 *     - int Accept();
 *     \snippet Client/ParameterCompone/ParameterUserUI.h Parameter commone functions
 * - 使用参数组件：
 *   - 在需要的地方实例化组件类。例如： CParameterNet
 *     \snippet Client/ParameterCompone/ParameterNet.h Instance user
 *     \snippet Client/ParameterCompone/ParameterNet.cpp Constructor
 *   - 在相应界面类
 *     - 初始化参数界面： CParameterNetUI::SetParameter 调用实例的 CParameterUserUI::SetParameter
 *     \snippet Client/ParameterCompone/ParameterNetUI.cpp Set Parameter
 *     - 检查参数的有效性： CParameterNetUI::CheckValidity
 *     \snippet Client/ParameterCompone/ParameterNetUI.cpp Check validity
 *     - 应用参数 CParameterNetUI::Accept 调用实例的 CParameterUserUI::Accept
 *     \snippet Client/ParameterCompone/ParameterNetUI.cpp Accept
 *
 * \~english
 * \defgroup CLIENT_PARAMETER_COMPONE Parameter compone
 * \brief Parameter compone.
 * - Write a parameter compone
 *   - Defined the parameter compone class. eg: CParameterUser
 *   - Add the parameter compone UI is derived from CParameterUI . eg: CParameterUserUI .
 *     and add the follow functions:
 *     - int SetParameter(CParameter* pParameter);
 *     - int Accept(bool validity = false);
 *     \snippet Client/ParameterCompone/ParameterUserUI.h Parameter commone functions
 * - Use a parameter compone
 *   - Instance the parameter compone class. eg: CParameterNet
 *     \snippet Client/ParameterCompone/ParameterNet.h Instance user
 *     \snippet Client/ParameterCompone/ParameterNet.cpp Constructor
 *   - Call SetParameter and slotAccept
 *     - CParameterNetUI::SetParameter call CParameterUserUI::SetParameter
 *     \snippet Client/ParameterCompone/ParameterNetUI.cpp Set Parameter
 *     - Check parameter validity: CParameterNetUI::CheckValidity
 *     \snippet Client/ParameterCompone/ParameterNetUI.cpp Check validity
 *     - CParameterNetUI::Accept call CParameterUserUI::Accept
 *     \snippet Client/ParameterCompone/ParameterNetUI.cpp Accept
 * \~
 * \ingroup CLIENT_PARAMETER
 */

/*!
 * \~chinese
 * \brief 连接参数接口。仅在插件内有效。
 *
 * \note
 *  - 仅在插件中实现和使用
 *  - 应用程序不能访问，只能通过 CConnecter::OpenDialogSettings 进行设置。
 *  - 插件通过 CConnecterThread::GetParameter() 访问连接参数
 *  - \ref section_Use_CParameterBase
 *  - 需要访问客户端参数 (CFrmParameterClient) 的分类，需要从此类派生。
 *    通过调用 CParameterConnecter::GetParameterClient()
 *    访问客户端参数(CParameterClient)
 *
 * \details
 * \section section_Use_CParameterClient 使用客户端库参数 (CParameterClient)
 * \subsection sub_Set_CParameterClient_in_CParameterConnecter 为连接参数 ( CParameterConnecter 或其派生类 ) 设置客户端库参数 (CParameterClient)
 *  - 如果连接参数 ( CParameterConnecter 或其派生类 ) 需要客户端库参数 (CParameterClient) 。
 *    - 请在 CConnecter 派生类的构造函数中实例化连接参数。例如：
 *      \snippet Plugins/FreeRDP/Client/ConnecterFreeRDP.h Initialize parameter
 *    - 调用 CConnecter::SetParameter 设置参数指针。例如：
 *      \snippet Plugins/FreeRDP/Client/ConnecterFreeRDP.cpp Set the parameter
 *    - 默认会自动为连接参数设置 CParameterClient 。详见: CClient::CreateConnecter 。
 *      \snippet Client/Client.cpp CClient CreateConnecter
 *    - 如果参数不需要 CParameterClient ，
 *      那请在 CConnecter 派生类重载 CConnecter::SetParameterClient 不设置它。
 * \subsection sub_Use_CParameterClient_in_CParameterConnecter 在连接参数 ( CParameterConnecter 或其派生类 ) 中使用客户端库参数 (CParameterClient)
 *   - 在连接参数 ( CParameterConnecter 或其派生类 ) 使用 CParameterConnecter::GetParameterClient() 来得到客户端库参数 (CParameterClient) 。
 *   - 连接参数 ( CParameterConnecter 或其派生类 ) 使用客户端库参数 (CParameterClient) 的值做为其初始值。
 *     请重载 CParameterConnecter::slotSetParameterClient 并在其中初始化相关的值。\n
 *     例如：是否保存密码以 CParameterClient 的成员值做为其初始化值。
 *     \snippet Client/ParameterCompone/ParameterUser.cpp Initialize parameter after set CParameterClient
 *     \see CParameterUser::OnLoad
 *
 * \section section_CParameterConnecter_CATEGORY_USAGE 连接参数分类使用
 *  因为可能会有很多参数，所以需要按参数类型分类。每个分类可以从此类派生出一个单独的类。\n
 *  因为连接参数都有一些基本的参数（例如网络地址等），所以插件的连接参数从 CParameterBase 派生。
 *  而每个分类做为它的成员变量。
 *
 *  例如：\n
 *  连接参数包括以下几种类型：
 *  - 基本参数
 *    \code
 *    class CParameterBase : public CParameterConnecter
 *    \endcode
 *  - 用户参数
 *    \code
 *    class CParameterUser : public CParameterConnecter
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
 *  其中 CParameterBase ， CParameterUser 它需要 CFrmParameterClient ，
 *  所以必须从 CParameterConnecter 派生。
 *  其它的类型则不需要，所以从 CParameter 派生。
 *
 *  那么连接参数可以是以上类型的集合：
 *
 *  \code
 *  class CParameterConnect : public CParameterBase
 *  {
 *  public:
 *      explicit CParameterConnect(CParameterConnecter *parent = nullptr);
 *
 *      CParameterUser m_User;
 *      CParameterVideo m_Video;
 *      CParameterAudio m_Audio;
 *  };
 *
 *  CParameterConnect::CParameterConnect(CParameterConnecter *parent = nullptr)
 *   : CParameterBase(parent),
 *     m_User(this, "Host"),
 *     m_Video(this),
 *     m_Audio(this)
 *  {}
 *  \endcode
 *
 *  \note
 *  - 连接参数必须从 CParameterBase 派生
 *  - 当成员实例化时
 *    - CParameterUser 必须设置构造函数的参数 parent
 *      为 CParameterConnecter 的实例（一般设置为 this）。
 *    - m_Video ， m_Audio 必须设置构造函数的参数 parent
 *      为 CParameter 或其派生类的实例（一般设置为 this）。
 *  - \ref sub_Set_CParameterClient_in_CParameterConnecter
 *
 * \~english
 * \brief the parameters of connecter interface. It only valid in plugin.
 *        
 * \note
 *  - The interface only is implemented and used by plugin.
 *  - The application cannot access it directly,
 *    it can only be set via CConnecter::OpenDialogSettings.
 *  - The plugin can access via CConnecterThread::GetParameter
 *  - Need access the client parameters(CParameterClient),
 *    must be derived from the class.
 *    Access the client parameters(CParameterClient)
 *    via CParameterConnecter::GetParameterClient()
 *  - \ref section_Use_CParameterBase
 *
 * \details
 * \section section_Use_CParameterClient Use CParameterClient
 * \subsection sub_Set_CParameterClient_in_CParameterConnecter Set CParameterClient for CParameterConnecter
 *  - If the parameters of connecter(CParameterConnecter or it's derived class)
 *    requires a CParameterClient.
 *    - Please instantiate the parameters of connecter
 *      in the constructor of the CConnecter or it's derived class. ag:
 *      \snippet Plugins/FreeRDP/Client/ConnecterFreeRDP.h Initialize parameter
 *    - Call CConnecter::SetParameter to set the parameters. ag:
 *      \snippet Plugins/FreeRDP/Client/ConnecterFreeRDP.cpp Set the parameter
 *    - Default set the CParameterClient for the parameters of connecter.
 *      See: CClient::CreateConnecter .
 *      \snippet Client/Client.cpp CClient CreateConnecter
 *    - If you are sure to the parameter does not need CParameterClient.
 *      please overload the CConnecter::SetParameterClient
 *      in the CConnecter derived class, don't set it.
 * \subsection sub_Use_CParameterClient_in_CParameterConnecter Use CParameterClient in CParameterConnecter
 *  - Use CParameterConnecter::GetParameterClient() get CParameterClient
 *    in the parameters of connecter(CParameterConnecter or it's derived class)
 *  - The parameters of connecter(CParameterConnecter or it's derived class)
 *    use the value of CParameterClient as its initial value. \n
 *    Please override CParameterConnecter::slotSetParameterClient
 *    and initialize the relevant values in it.\n
 *    For example, Whether to save the password
 *    with the member value of CParameterClient as its initialization value.
 *    \snippet Client/ParameterCompone/ParameterUser.cpp Initialize parameter after set CParameterClient
 *    \see CParameterUser::OnLoad
 * \section section_CParameterConnecter_CATEGORY_USAGE CParameterConnecter category
 *  Because there may be many parameters,
 *  it is necessary to classify them by parameter category.
 *  Each category can derive a separate class from this class.
 *  Each category is then used as a member variable in the derived classes of this class.
 *
 *  For example:\n
 *  Connection parameters include the following categories:
 *  - Base
 *    \code
 *    class CParameterBase : public CParameterConnecter
 *    \endcode
 *  - User
 *    \code
 *    class CParameterUser : public CParameterConnecter
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
 *  CParameterBase, CParameterUser need CFrmParameterClient,
 *  so that it must derived from CParameterConnecter.
 *  other is derived from CParameter.
 *  
 *  Then the connection parameters can be a combination of the above categories:
 *
 *  \code
 *  class CParameterConnect : public CParameterBase
 *  {
 *  public:
 *      explicit CParameterConnect(CParameterConnecter *parent = nullptr);
 *
 *      CParameterUser m_User;
 *      CParameterVideo m_Video;
 *      CParameterAudio m_Audio;
 *  };
 *
 *  CParameterConnect::CParameterConnect(CParameterConnecter *parent = nullptr)
 *   : CParameterBase(parent),
 *     m_User(this, "Host"),
 *     m_Video(this),
 *     m_Audio(this)
 *  {}
 *  \endcode
 *
 *  \note
 *  - The connect parameter must be derived from CParameterBase
 *  - When a member is instantiated,
 *   the constructor must set the parent to the instance of CParameterConnecter(this).
 *  - \ref sub_Set_CParameterClient_in_CParameterConnecter
 *
 * \~
 * \see CParameterBase
 *      CClient::CreateConnecter
 *      CConnecter::CConnecter
 *      CConnecter::SetParameterClient
 *      CConnecter::SetParameter
 *      CParameterClient
 *      CParameterConnecter::GetParameterClient
 * \ingroup CLIENT_PARAMETER
 */
class PLUGIN_EXPORT CParameterConnecter : public CParameter
{
    Q_OBJECT

public:
    explicit CParameterConnecter(
        QObject *parent = nullptr,
        const QString& szPrefix = QString());

    //! Get CParameterClient
    CParameterClient* GetParameterClient();
    int SetParameterClient(CParameterClient* p);

Q_SIGNALS:
    void sigSetParameterClient();

protected Q_SLOTS:
    /*!
     * \~chinese 设置 CParameterClient 后调用，用于初始化与 CParameterClient 有关的操作。
     *
     * \~english Call after set CParameterClient.
     * Used to initialize operations related to CParameterClient.
     *
     * \~
     * \ref sub_Use_CParameterClient_in_CParameterConnecter
     */
    virtual void slotSetParameterClient();

protected:
    QByteArray PasswordSum(const std::string &password, const std::string &key);
    int LoadPassword(const QString &szTitle, const QString &szKey,
                     QString &password, QSettings &set);
    int SavePassword(const QString &szKey, const QString &password,
                     QSettings &set, bool bSave = false);

private:
    CParameterConnecter* m_Parent;
    /*!
     * \see CClient::CreateConnecter CConnecter::SetParameterClient
     */
    CParameterClient* m_pParameterClient;
};

#endif // CparameterCONNECT_H
