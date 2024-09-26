#ifndef CPARAMETERNET_H
#define CPARAMETERNET_H

#include <QObject>

#include "ParameterUser.h"
#include "ParameterWakeOnLan.h"

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
 * \see CParameterConnecter CParameterNetUI
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class CLIENT_EXPORT CParameterNet
    : public CParameterConnecter
{
    Q_OBJECT

public:
    explicit CParameterNet(CParameterConnecter* parent,
                           const QString& szPrefix = QString());
    
    virtual const QString GetHost() const;
    virtual void SetHost(const QString& szHost);
    
    virtual const quint16 GetPort() const;
    virtual void SetPort(quint16 port);
    
    //! [Instance user]
    CParameterUser m_User;
    //! [Instance user]
    CParameterWakeOnLan m_WakeOnLan;
    
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

#endif // CPARAMETERNET_H
