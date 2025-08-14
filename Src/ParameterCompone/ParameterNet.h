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
};
