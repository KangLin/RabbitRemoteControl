// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QSettings>
#include <QString>
#include <QVector>
#include "plugin_export.h"

/*!
 * \~chinese
 * \defgroup CLIENT_PARAMETER 参数接口
 * \brief 参数接口
 * \details
 * - 参数有以下类型：
 *   1. 仅在插件内有效。
 *      应用程序不能访问，但是可以通过 COperate::OpenDialogSettings 进行设置。
 *      \see 
 *        \ref section_CParameterOperate_CATEGORY_USAGE
 *        CParameterOperate
 *        CParameterBase
 *   2. 在插件管理者 (CManager) 和插件内有效。\n
 *      应用程序不能访问，但是可以通过 CManager::GetSettingsWidgets 进行设置。\n
 *      插件管理者 (CManager) 和插件可以直接使用。插件可以以其做为初始化值。
 *      \ref section_Use_CParameterPlugin
 *      \see CParameterPlugin
 *           COperate::SetGlobalParameters
 *   3. 同时在应用程序、插件管理者 (CManager) 和插件内有效。暂时没有使用此类型。
 * - 参数界面： CParameterUI
 *
 * \~english
 * \defgroup CLIENT_PARAMETER Parameter interface
 * \brief Parameter interface
 * \details
 * - The parameter has the following type:
 *   1. Only valid in the plugin.
 *      The application cannot access it,
 *      but the application can be set it via COperate::OpenDialogSettings.
 *      \see
 *        \ref section_CParameterOperate_CATEGORY_USAGE
 *        CParameterOperate
 *        CParameterBase
 *   2. Valid in the CManager and the plugin.\n
 *      The application cannot access it,
 *      but the application can be set it via CManager::GetSettingsWidgets.\n
 *      CManager and Plugins can use them directly.
 *      The plugin maybe use them as initialization values.
 *      \ref section_Use_CParameterPlugin
 *      \see CParameterClient
 *           COperate::SetGlobalParameters
 *   3. Valid in both the application and the Client and the plugin.
 *      This type is not used at this time.
 * - CParameterUI
 * \~
 * \ingroup LIBAPI_CLIENT
 */

/*!
 * \~chinese
 * \defgroup CLIENT_PARAMETER_COMPONE 参数组件
 * \brief 参数组件。
 * - 写一个参数组件：
 *   - 定义参数组件类(从 CParameter 或其派生类派生)。例如： CParameterUser 。
 *   - 增加参数组件界面，从 CParameterUI 派生。例如： CParameterUserUI 。 并重载下面函数：
 *     - int SetParameter(CParameter* pParameter);
 *     - int Accept();
 *     \snippet Src/ParameterCompone/ParameterUserUI.h Parameter commone functions
 * - 使用参数组件：
 *   - 在需要的地方实例化组件类。例如： CParameterNet
 *     \snippet Src/ParameterCompone/ParameterNet.h Instance user
 *     \snippet Src/ParameterCompone/ParameterNet.cpp Constructor
 *   - 在相应界面类
 *     - 初始化参数界面： CParameterNetUI::SetParameter 调用实例的 CParameterUserUI::SetParameter
 *     \snippet Src/ParameterCompone/ParameterNetUI.cpp Set Parameter
 *     - 检查参数的有效性： CParameterNetUI::CheckValidity
 *     \snippet Src/ParameterCompone/ParameterNetUI.cpp Check validity
 *     - 应用参数 CParameterNetUI::Accept 调用实例的 CParameterUserUI::Accept
 *     \snippet Src/ParameterCompone/ParameterNetUI.cpp Accept
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
 *     \snippet Src/ParameterCompone/ParameterUserUI.h Parameter commone functions
 * - Use a parameter compone
 *   - Instance the parameter compone class. eg: CParameterNet
 *     \snippet Src/ParameterCompone/ParameterNet.h Instance user
 *     \snippet Src/ParameterCompone/ParameterNet.cpp Constructor
 *   - Call SetParameter and slotAccept
 *     - CParameterNetUI::SetParameter call CParameterUserUI::SetParameter
 *     \snippet Src/ParameterCompone/ParameterNetUI.cpp Set Parameter
 *     - Check parameter validity: CParameterNetUI::CheckValidity
 *     \snippet Src/ParameterCompone/ParameterNetUI.cpp Check validity
 *     - CParameterNetUI::Accept call CParameterUserUI::Accept
 *     \snippet Src/ParameterCompone/ParameterNetUI.cpp Accept
 * \~
 * \ingroup CLIENT_PARAMETER
 */

/*!
 * \~chinese 参数接口
 * \details
 * \section sub_CParameter_CATEGORY_USAGE 分类使用
 *  因为可能会有很多参数，所以需要按分类来管理。每个分类可以从此类派生出一个单独的类。
 *  然后每个分类再做为参数的成员变量。
 *
 *  例如：\n
 *  连接参数包括以下几种类型：
 *  - 网络参数
 *    \code
 *    class CParameterNet : public CParameter
 *    \endcode
 *  - 用户参数
 *    \code
 *    class CParameterUser : public CParameter
 *    \endcode
 *
 *  那么连接参数可以是以上类型的集合：
 *
 *  \code
 *  class CParameterConnect : public CParameter
 *  {
 *  public:
 *      explicit CParameterConnect(CParameter *parent = nullptr);
 *
 *      CParameterNet m_Net;
 *      CParameterUser m_User;
 *  };
 *
 *  CParameterConnect::CParameterConnect(CParameter *parent = nullptr)
 *   : CParameter(parent),
 *     m_Net(this), // 允许自动嵌套调用 CParameter 类型成员的 OnXXX 函数
 *     m_User(this)
 *  {}
 *  \endcode
 *
 *  \note 当成员实例化时，必须设置构造函数的参数 parent 为 this
 *
 * \section CParameter_Functions CParameter 接口功能
 *  - 从存储中加载或保存参数到存储中
 *    - 如果要自动嵌套调用 CParameter 类型成员的 OnXXX 函数（例如: OnLoad)，
 *      在成员实例化时，设置构造函数的 parent 为 CParameter(一般用this） 类型实例。
 *    - 如果要自己控制 CParameter 类型成员的 OnXXX 函数（例如: OnLoad)，
 *      在成员实例化时，设置构造函数的 parent 为空或者设置为非 CParameter 类型实例。
 *  - 当派生类设置参数时，需要调用 SetModified 。标志参数已改变。
 *  - 检查参数的有效性。调用 CheckValidity()
 *
 * \~english Parameter interface
 * \section sub_CParameter_CATEGORY_USAGE Parameter category
 *  Because there may be many parameters,
 *  it is necessary to classify them by parameter category.
 *  Each category can derive a separate class from this class.
 *  Each category is then used as a member variable in the derived classes of this class.
 *
 *  For example:\n
 *  Connection parameters include the following categories:
 *  - Video
 *    \code
 *    class CParameterNet : public CParameter
 *    \endcode
 *  - Audio
 *    \code
 *    class CParameterUser : public CParameter
 *    \endcode
 *
 *  Then the connection parameters can be a combination of the above categories:
 *
 *  \code
 *  class CParameterConnect : public CParameter
 *  {
 *  public:
 *      explicit CParameterConnect(CParameter *parent = nullptr);
 *
 *      CParameterNet m_Net;
 *      CParameterUser m_User;
 *  };
 *
 *  CParameterConnect::CParameterConnect(CParameter *parent = nullptr)
 *   : CParameter(parent),
 *     m_Net(this),
 *     m_User(this)
 *  {}
 *  \endcode
 *
 *  \note When a member is instantiated,
 *   the constructor must set the parent to *this*
 *
 * \section CParameter_Functions CParameter Interface functions
 *  - Load and save parameters from storage
 *    - If you need to automatically nest calls the OnXXX(ag: OnLoad) functions
 *      of CParameter type member,
 *      When a CParameter type member is instantiated,
 *      the constructor sets the parent to a CParameter (usually this) type instance.
 *    - If you're going to control the OnXXX(ag: OnLoad) functions
 *      of CParameter type member,
 *      When a CParameter type member is instantiated,
 *      the constructor parent is set nullptr or non-CParameter type instance.
 *  - When the derived class sets parameters,
 *    it needs to call SetModified(), to flag changed.
 *  - Check whether the parameter is valid, call CheckValidity()
 *
 * \~
 * \ingroup CLIENT_PARAMETER CLIENT_PLUGIN_API
 */
class PLUGIN_EXPORT CParameter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool Modified READ GetModified WRITE SetModified FINAL)

public:
    /*!
     * \~chinese
     * \param parent: 
     *   - 为 CParameter 实例，则自动调用 CParameter 类型成员的 OnXXX 函数。
     *   - 为空或者非 CParameter 实例，则不调用 CParameter 类型成员的 OnXXX 函数
     *   \see Load(QSettings &set) Save(QSettings &set, bool bForce = true)
     *
     * \~english
     * \param parent:
     *   - An instance of this class or its derivative class, then
     *     automatically nest calls the OnXXX(ag: OnLoad) functions of member.
     *   - Is null or non-CParameter instance,
     *     then don't nest calls the OnXXX(ag: OnLoad) functions of member.
     *   \see Load(QSettings &set) Save(QSettings &set, bool bForce = true)
     * \param szPrefix Prefix \see QSetting::beginGroup
     * \param szPrefix: 前缀。 \see QSetting::beginGroup
     */
    explicit CParameter(QObject *parent = nullptr,
                        const QString& szPrefix = QString());
    virtual ~CParameter();

    //! Load from file
    virtual int Load(QString szFile = QString());
    //! Save to file
    virtual int Save(QString szFile = QString(), bool bForce = true);
    
    //! Load from QSettings
    virtual int Load(QSettings &set);
    //! Save to QSettings
    virtual int Save(QSettings &set, bool bForce = true);

    /*!
     * \~chinese
     *  检查参数是否有效，以决定是否使用或者保存参数。
     *  派生类一般只要重载 OnCheckValidity() ，用于检查参数。
     *
     *  例如：用于在参数设置对话框时，需要先检查参数是否完成或者有效，才能确定保存。
     *
     * \~english
     *  Check whether the parameter is valid to
     *  decide whether to use or save the parameter.\n
     *  Derived classes typically simply overload OnCheckValidity()
     *  to check the parameters.
     *
     *  For example, when setting the parameters in the dialog,
     *  you need to check whether the parameters are complete
     *  or valid before you can save them.
     *
     * \~ \see CParameterUI::CheckValidity
     */
    virtual bool CheckValidity();

Q_SIGNALS:
    /*!
     * \~chinese 当参数改变时，触发
     *  通常如果需要，则相应的参数会对应一个改变事件。
     *
     * \~english emit when the parameter changes
     * Usually if required, the corresponding parameter corresponds to
     * a change event.
     */
    void sigChanged();

protected:
    virtual int OnLoad(QSettings &set) = 0;
    virtual int OnSave(QSettings &set) = 0;
    /*!
     * \~chinese
     * \brief 检查参数是否有效
     * \return
     *   - true: 有效
     *   - false: 无效
     *   
     * \~english
     * \brief Check validity
     * \return
     *   - true: valid
     *   - false: invalid
     */
    virtual bool OnCheckValidity();

    /*!
     * \~chinese
     * \brief 在设置参数时，如果有修改，则调用。
     * \~english
     * \brief When setting parameters, if there is a modification, it is called
     * \~
     * \param bModified
     */
    int SetModified(bool bModified = true);
    /*!
     * \~chinese 参数是否有修改
     * \~english Whether the parameters have been modified
     * \~
     * \brief GetModified
     * \return
     *  - true: modified
     *  - false: no modified
     */
    bool GetModified();

private:
    /*!
     * \~chinese 此类及其派生类的实例做为实例的成员
     * \~english Instances of this class and its derived classes are
     *           members of the instance
     */
    int AddCategory(CParameter* p);
    QString GetPrefix() const;
    int SetPrefix(const QString& szPrefix);

private:
    QString m_szPrefix;

    /*!
     * \~chinese
     * \brief 标识参数是否有修改。如果 false，则保存文件时，不保存
     * \~english
     * \brief If false, then don't save when save.
     */
    bool m_bModified;

    /*!
     * \~chinese 类别
     * \~english Category
     */
    QVector<CParameter*> m_Category;
};
