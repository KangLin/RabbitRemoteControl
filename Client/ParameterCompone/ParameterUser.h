#ifndef CPARAMETERUSERPASSWORD_H
#define CPARAMETERUSERPASSWORD_H

#include "ParameterConnecter.h"

/*!
 * \~english
 * \brief It contains user and password
 *        It only valid in plugin.
 * \note
 *  - The interface only is implemented and used by plugin.
 *
 * \~chinese
 * \brief
 *  用户名与密码。此类仅在插件内有效。
 *
 * \~
 * \see CParameterConnecter
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class CLIENT_EXPORT CParameterUser : public CParameterConnecter
{
    Q_OBJECT

public:
    explicit CParameterUser(CParameterConnecter* parent,
                            const QString& szPrefix = QString());

    virtual const QString GetUser() const;
    virtual void SetUser(const QString& szUser);

    virtual const QString GetPassword() const;
    virtual void SetPassword(const QString& szPassword);

    const bool GetSavePassword() const;
    /*!
     * \brief Set save password
     * \param save
     */
    void SetSavePassword(bool save);

protected:
    virtual int onLoad(QSettings &set) override;
    virtual int onSave(QSettings &set) override;

private:
    QString m_szUser;
    QString m_szPassword;
    bool m_bSavePassword;

    // CParameterConnecter interface
protected:
    virtual int slotSetParameterClient() override;
};

#endif // CPARAMETERUSERPASSWORD_H
