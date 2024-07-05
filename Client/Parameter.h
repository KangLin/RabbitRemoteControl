// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMETER_H_KL_2022_07_27
#define CPARAMETER_H_KL_2022_07_27

#pragma once

#include <QSettings>
#include <QString>
#include <QVector>
#include "client_export.h"

/*!
 * \~chinese
 * \defgroup CLIENT_PARAMETER 参数接口
 * \brief 参数接口
 * \details
 *   参数有以下类型：
 *   1. 仅在插件内有效。
 *      应用程序不能访问，但是可以通过 CConnecter::OpenDialogSettings 进行设置。
 *      \see 
 *        \ref section_CParameterConnecter_CATEGORY_USAGE
 *        CParameterConnecter
 *        CParameterBase
 *   2. 在客户端库 (CClient) 和插件内有效。\n
 *      应用程序不能访问，但是可以通过 CClient::GetSettingsWidgets 进行设置。\n
 *      客户端 (CClient) 和插件可以直接使用。插件可以以其做为初始化值。
 *      \ref section_Use_CParameterClient
 *      \see CParameterClient
 *           CConnecter::SetParameterClient
 *           CParameterUser::OnLoad
 *   3. 同时在应用程序、客户端库 (CClient) 和插件内有效。暂时没有使用此类型。
 *
 * \~english
 * \defgroup CLIENT_PARAMETER Parameter interface
 * \brief Parameter interface
 * \details
 *  The parameter has the following type:
 *  1. Only valid in the plugin.
 *     The application cannot access it,
 *     but the application can be set it via CConnecter::OpenDialogSettings.
 *      \see 
 *        \ref section_CParameterConnecter_CATEGORY_USAGE
 *        CParameterConnecter
 *        CParameterBase
 *  2. Valid in the CClient and the plugin.\n
 *     The application cannot access it,
 *     but the application can be set it via CClient::GetSettingsWidgets.\n
 *     CClient and Plugins can use them directly.
 *     The plugin maybe use them as initialization values.
 *     \ref section_Use_CParameterClient
 *     \see CParameterClient
 *          CConnecter::SetParameterClient
 *          CParameterUser::OnLoad
 *  3. Valid in both the application and the Client and the plugin.
 *     This type is not used at this time.
 * \~
 * \ingroup LIBAPI_CLIENT
 */

/*!
 * \~chinese 参数接口
 * \details
 * \section sub_CParameter_CATEGORY_USAGE 参数分类使用
 *  因为可能会有很多参数，所以需要按参数类型分类来管理。每个分类可以从此类派生出一个单独的类。
 *  然后每个分类再做为参数的成员变量。
 *
 *  例如：\n
 *  连接参数包括以下几种类型：
 *  - 视频参数 (class CParameterVideo : public CParameter)
 *  - 音频参数 (class CParameterAudio : public CParameter)
 *
 *  那么连接参数可以是以上类型的集合：
 *
 *  \code
 *  class CParameterConnect : public CParameter
 *  {
 *  public:
 *      explicit CParameterConnect(CParameter *parent = nullptr);
 *
 *      CParameterVideo m_Video;
 *      CParameterAudio m_Audio;
 *  };
 *
 *  CParameterConnect::CParameterConnect(CParameter *parent = nullptr)
 *   : CParameter(parent),
 *     m_Video(this),
 *     m_Audio(this)
 *  {}
 *  \endcode
 *
 *  \note 当成员实例化时，必须设置构造函数的参数 parent 为 this
 *
 * \section CParameter_Functions CParameter 接口功能
 *  - 从存储中加载和保存参数
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
 *  - Video (class CParameterVideo : public CParameter)
 *  - Audio (class CParameterAudio : public CParameter)
 *
 *  Then the connection parameters can be a combination of the above categories:
 *
 *  \code
 *  class CParameterConnect : public CParameter
 *  {
 *  public:
 *      explicit CParameterConnect(CParameter *parent = nullptr);
 *
 *      CParameterVideo m_Video;
 *      CParameterAudio m_Audio;
 *  };
 *
 *  CParameterConnect::CParameterConnect(CParameter *parent = nullptr)
 *   : CParameter(parent),
 *     m_Video(this),
 *     m_Audio(this)
 *  {}
 *  \endcode
 *
 *  \note When a member is instantiated,
 *   the constructor must set the parent to this
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
class CLIENT_EXPORT CParameter : public QObject
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

    virtual int Load(QString szFile = QString());
    virtual int Save(QString szFile = QString(), bool bForce = true);
    
    //! Load from storage
    virtual int Load(QSettings &set);
    //! Save to storage
    virtual int Save(QSettings &set, bool bForce = true);

    /*!
     * \~chinese
     *  检查参数是否有效，以决定是否应用或者保存参数。
     *  派生类一般只要重载 onCheckValidity() ，用于检查参数。
     *
     *  通常这种检查在参数设置对话框视图中做检查。所以很少需要调用此函数。
     *
     *  例如：用于在参数设置对话框时，需要先检查参数是否完成或者有效，才能确定保存。
     *  否则提示参数无效。
     *
     * \~english
     *  Check whether the parameter is set or valid to
     *  decide whether to apply or save the parameter.
     *  The derived class needs to overload onCheckValidity()
     *  as needed to check the parameters.
     *
     *  Usually this check is done in the Parameter Settings dialog view.
     *  So this function is rarely called in person.
     *
     *  For example, when setting the parameters in the dialog box,
     *  you need to check whether the parameters are complete or valid
     *  before you can save them.
     */
    virtual bool CheckValidity();
    
Q_SIGNALS:
    /*!
     * \~chinese 当参数改变时，触发
     *  通常如果需要，则相应的参数会对应一个改变事件。所以很少需要连接此信号。
     *
     * \~english emit when the parameter changes
     * Usually if required, the corresponding parameter corresponds to
     * a change event. So it is rarely necessary to connect this signal.
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

#endif // CPARAMETER_H_KL_2022_07_27
