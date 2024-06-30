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
 *      \see CParameterConnecter CParameterBase
 *   2. 在客户端库 (CClient) 和插件内有效。\n
 *      应用程序不能访问，但是可以通过 CClient::GetSettingsWidgets 进行设置。\n
 *      客户端 (CClient) 和插件可以直接使用。插件可以以其做为初始化值。
 *      \ref sub_Use_CParameterClient
 *      \see CParameterClient
 *           CConnecter::SetParameterClient
 *           CParameterUser::onLoad
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
 *     \see CParameterConnecter
 *  2. Valid in the CClient and the plugin.\n
 *     The application cannot access it,
 *     but the application can be set it via CClient::GetSettingsWidgets.\n
 *     CClient and Plugins can use them directly.
 *     The plugin maybe use them as initialization values.
 *     \ref sub_Use_CParameterClient
 *     \see CParameterClient
 *          CConnecter::SetParameterClient
 *          CParameterUser::onLoad
 *  3. Valid in both the application and the Client and the plugin.
 *     This type is not used at this time.
 * \~
 * \ingroup LIBAPI_CLIENT
 */

/*!
 * \~chinese 参数接口
 * \details
 * - 如果你需要自动嵌套调用 CParameter::onLoad 或 CParameter::onSave ，
 *   请用构造函数 CParameter(CParameter* parent, const QString& szPrefix) 初始化实例。
 *
 * \~english Parameter interface
 * - If you need to automatically nest calls CParameter::onLoad or CParameter::onSave,
 *   Please initialize the instance with CParameter(CParameter* parent, const QString& szPrefix).
 * \~
 * \ingroup CLIENT_PARAMETER CLIENT_PLUGIN_API
 */
class CLIENT_EXPORT CParameter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool Modified READ GetModified WRITE SetModified FINAL)

public:
    explicit CParameter(QObject *parent = nullptr);
    /*!
     * \~english
     * \brief
     *  This object, or its derivative,
     *  is a member of another instance of this class,
     *  and its Load and Save are automatically called
     * \param parent An instance of this class or its derivative class
     *
     * \~chinese
     * \brief 此对象或者其派生对象作为另一个此类对象的实例的成员，则自动调用其 Load 和 Save
     * \param parent 此类或其派生类的实例
     * \param szPrefix 前缀。 \see QSetting::beginGroup
     */
    explicit CParameter(CParameter* parent, const QString& szPrefix);
    virtual ~CParameter();

    virtual int Load(QString szFile = QString());
    virtual int Save(QString szFile = QString(), bool bForce = true);

    virtual int Load(QSettings &set);
    virtual int Save(QSettings &set, bool bForce = true);

    /*!
     * \~chinese
     *  检查参数是否设置完成或者有效，以决定是否应用或者保存参数。
     *  派生类需要根据需要重载此函数，用于检查参数。
     *
     *  通常这种检查在参数设置对话框视图中做检查。所以很少需要调用此函数。
     *
     *  例如：用于在参数设置对话框时，需要先检查参数是否完成或者有效，才能确定保存。
     *  否则提示参数无效。
     *
     * \~english
     *  Check whether the parameter is set or valid to
     *  decide whether to apply or save the parameter.
     *  The derived class needs to overload this function
     *  as needed to check the parameters.
     *
     *  Usually this check is done in the Parameter Settings dialog view.
     *  So this function is rarely called in person.
     *
     *  For example, when setting the parameters in the dialog box,
     *  you need to check whether the parameters are complete or valid
     *  before you can save them.
     */
    virtual bool CheckCompleted();
    
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
    virtual int onLoad(QSettings &set) = 0;
    virtual int onSave(QSettings &set) = 0;

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
     * \~english Instances of this class and its derived classes are
     *           members of the instance
     *
     * \~chinese
     * \brief 此类及其派生类的实例做为实例的成员
     */
    int AddMember(CParameter* p);
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
     * \~english Instances of this class and its derived classes are
     *           members of the instance
     *
     * \~chinese
     * \brief 此类及其派生类的实例做为实例的成员
     */
    QVector<CParameter*> m_Member;
};

#endif // CPARAMETER_H_KL_2022_07_27
