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
 * \brief 用户名与验证方式。此类仅在插件内有效。
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
    
    enum class TYPE{
        None,
        OnlyPassword,
        UserPassword,
        PublicKey
    };
    QList<TYPE> GetType() const;
    int SetType(QList<TYPE> type);
    TYPE GetUsedType() const;
    int SetUsedType(TYPE type);

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
    
    bool GetUseSystemFile() const;
    int SetUseSystemFile(bool use);

    QString GetPublicKeyFile() const;
    int SetPublicKeyFile(const QString szFile);
    
    QString GetPrivateKeyFile() const;
    int SetPrivateKeyFile(const QString szFile);
    
    QString GetPassphrase() const;
    int SetPassphrase(const QString passphrase);
    
    bool GetSavePassphrase() const;
    int SetSavePassphrase(bool bSave);
    
    void SetConvertTypeToNameCallBack(std::function<QString(TYPE t)> cb);
    QString ConvertTypeToName(TYPE t);

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
    
    // CParameterConnecter interface
    virtual void slotSetParameterClient() override;
    
private:
    QList<TYPE> m_Type;
    TYPE m_UsedType;

    QString m_szUser;
    
    // Password
    QString m_szPassword;
    bool m_bSavePassword;
    
    // Public key
    bool m_bUseSystemFile;
    QString m_szPublicKeyFile;
    QString m_szPrivateKeyFile;
    QString m_szPassphrase;
    bool m_bSavePassphrase;
    
    std::function<QString(TYPE t)> m_cbConvertTypeToName;
    QVector<QString> m_TypeName;
};

#endif // CPARAMETERUSERPASSWORD_H
