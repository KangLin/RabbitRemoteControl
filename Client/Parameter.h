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
    Q_PROPERTY(bool Modified READ GetModified WRITE SetModified FINAL)

public:
    explicit CParameter(QObject *parent = nullptr);
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
     *  通常如果需要，则相应的参数会对应一个改变事件。所以很少需要触发此信号。
     *
     * \~english emit when the parameter changes
     * Usually if required, the corresponding parameter corresponds to
     * a change event. So it is rarely necessary to trigger this signal.
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
     * \~chinese
     * \brief 标识参数是否有修改。如果 false，则保存文件时，不保存
     * \~english
     * \brief If false, then don't save when save.
     */
    bool m_bModified;
};

#endif // CPARAMETER_H_KL_2022_07_27
