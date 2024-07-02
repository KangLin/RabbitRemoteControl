#ifndef CPARAMETERNET_H
#define CPARAMETERNET_H

#include <QObject>

#include "ParameterUser.h"

/*!
 * \~english
 * \brief The parameter interface. It contains basic network parameters.
 *        It only valid in plugin.
 * \note
 *  - The interface only is implemented and used by plugin.
 *
 * \~chinese
 * \brief
 *  网络连接参数接口。此类仅在插件内有效。
 *
 * \~
 * \see CParameterConnecter
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

    CParameterUser m_User;

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

private:
    QString m_szHost;
    quint16 m_nPort;

};

#endif // CPARAMETERNET_H
