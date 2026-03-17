// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QObject>

#include "ParameterUser.h"

/*!
 * \~english
 * \brief Basic network parameters.
 *        It only valid in plugin.
 *        It's UI is CParameterNetUI
 * \note
 *  - The interface only is implemented and used by plugin.
 *
 * \~chinese
 * \brief
 *  网络连接参数接口。此类仅在插件内有效。
 *  其界面为 CParameterNetUI
 *
 * \~
 * \see CParameterOperate CParameterNetUI
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class PLUGIN_EXPORT CParameterNet
    : public CParameterOperate
{
    Q_OBJECT

public:
    explicit CParameterNet(CParameterOperate* parent,
                           const QString& szPrefix = QString());
    virtual CParameterNet& operator =(const CParameterNet& in);

    virtual const QString GetHost() const;
    virtual void SetHost(const QString& szHost);
Q_SIGNALS:
    void sigHostChanged(const QString& szHost);

public:
    virtual const quint16 GetPort() const;
    virtual void SetPort(quint16 port);

    //! [Instance user]
    CParameterUser m_User;
    //! [Instance user]

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

private:
    QString m_szHost;
    quint16 m_nPort;

public:
    int SetPrompt(const QString szPrompt);
    QString GetPrompt();
private:
    QString m_szPrompt;
    
public:
    /*!
     * \~chinese 用于控制显示界面元素
     * \~english Used to control display interface elements
     */
    enum class SHOW_UI {
        Host = 0x01,
        Port = 0x02,
        User = 0x04,
        All = Host | Port | User
    };
    Q_ENUM(SHOW_UI)
    Q_DECLARE_FLAGS(SHOW_UIS, SHOW_UI)
    Q_FLAG(SHOW_UIS)
    SHOW_UIS GetShowUI();
    SHOW_UIS SetShowUI(SHOW_UIS ui);
    SHOW_UIS GetEnableUI();
    SHOW_UIS SetEnablleUI(SHOW_UIS ui);
private:
    SHOW_UIS m_ShowUI;
    SHOW_UIS m_EnableUI;
};

// 在类外部声明操作符（通常放在头文件末尾）
Q_DECLARE_OPERATORS_FOR_FLAGS(CParameterNet::SHOW_UIS)
