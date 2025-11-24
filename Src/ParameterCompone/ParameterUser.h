// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterOperate.h"

/*!
 * \~english
 * \brief It contains user and password
 *        It only valid in plugin.
 *        It's UI is CParameterUserUI.
 * \note
 *  - The interface only is implemented and used by plugin.
 *
 * \~chinese
 * \brief 用户名与验证方式。此类仅在插件内有效。它的界面是 CParameterUserUI
 *
 * \~
 * \see CParameterOperate CParameterUserUI
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class PLUGIN_EXPORT CParameterUser : public CParameterOperate
{
    Q_OBJECT

public:
    explicit CParameterUser(CParameterOperate* parent,
                            const QString& szPrefix = QString());
    
    enum class TYPE{
        None,
        OnlyPassword,
        UserPassword,
        PublicKey,
        /*!
         *  Internet X.509 Public Key Infrastructure Certificate
             and Certificate Revocation List (CRL) Profile
         * \see
         * - [rfc5280](https://www.rfc-editor.org/rfc/rfc5280)
         * - [X.509 证书](https://learn.microsoft.com/zh-cn/azure/iot-hub/reference-x509-certificates)
         * - [X.509 公钥证书的格式标准](https://linianhui.github.io/information-security/05-x.509/)
         */
        OnlyPasswordX509None,
        OnlyPasswordX509,
        UserPasswordX509None,
        UserPasswordX509
    };
    Q_ENUM(TYPE)
    QList<TYPE> GetType() const;
    int SetType(QList<TYPE> type);
    TYPE GetUsedType() const;
    int SetUsedType(TYPE type);

    const QString GetUser() const;
    void SetUser(const QString& szUser);

    const QString GetPassword() const;
    void SetPassword(const QString& szPassword);

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
    
    QString GetCAFile() const;
    int SetCAFile(const QString& ca);
    
    QString GetCRLFile() const;
    int SetCRLFile(const QString& crl);
    
    int SetTypeName(TYPE t, const QString& szName);
    QString ConvertTypeToName(TYPE t);

protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

protected Q_SLOTS:
    /*!
     * \brief Initialize this parameter based on the global parameter
     */
    virtual void slotSetGlobalParameters() override;
private Q_SLOTS:
    void slotLoadPassword();
    void slotLoadPassPhrase();
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
    
    QString m_szCAFile;
    QString m_szCRLFile;
    
    QMap<TYPE, QString> m_TypeName;
};
