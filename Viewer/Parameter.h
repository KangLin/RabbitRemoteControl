// Author: Kang Lin <kl222@126.com>

#ifndef CPARAMTER_H
#define CPARAMTER_H

#pragma once

#include "viewer_export.h"
#include <QObject>
#include <QDataStream>
#include <QSettings>

/**
 * @~english
 * @brief The parameter interface. It contains basic parameters.
 * @note The interface only is implemented and used by plugin
 * 
 * @~chinese
 * @brief 参数接口。它包含基本参数
 * @note 此接口仅由插件派生实现和使用
 * 
 * @~
 * @ingroup VIEWER_PLUGIN_API
 */
class VIEWER_EXPORT CParameter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString Name READ GetName WRITE SetName)
    Q_PROPERTY(QString Host READ GetHost WRITE SetHost)
    Q_PROPERTY(qint16 Port READ GetPort WRITE SetPort)
    Q_PROPERTY(QString User READ GetUser WRITE SetUser)
    Q_PROPERTY(QString Password READ GetPassword WRITE SetPassword)
    Q_PROPERTY(bool SavePassword READ GetSavePassword WRITE SetSavePassword)
    Q_PROPERTY(bool OnlyView READ GetOnlyView WRITE SetOnlyView)
    Q_PROPERTY(bool LocalCursor READ GetLocalCursor WRITE SetLocalCursor)
    Q_PROPERTY(bool Clipboard READ GetClipboard WRITE SetClipboard)
    Q_PROPERTY(emProxy ProxyType READ GetProxyType WRITE SetProxyType)
    Q_PROPERTY(QString ProxyHost READ GetProxyHost WRITE SetProxyHost)
    Q_PROPERTY(qint16 ProxyPort READ GetProxyPort WRITE SetProxyPort)
    Q_PROPERTY(QString ProxyUser READ GetProxyUser WRITE SetProxyUser)
    Q_PROPERTY(QString ProxyPassword READ GetProxyPassword WRITE SetProxyPassword)
    
public:
    explicit CParameter(QObject *parent = nullptr);

    virtual int Load(QSettings &set);
    virtual int Save(QSettings &set);
    
    /*!
     * \brief Check whether the parameters are complete
     *  to decide whether to open the parameter dialog 
     * \return 
     */
    virtual bool GetComplete();
    
    const QString GetName() const;
    void SetName(const QString& name);
    
    const QString GetHost() const;
    void SetHost(const QString& szHost);
    
    const quint16 GetPort() const;
    void SetPort(quint16 port);
    
    const QString GetUser() const;
    void SetUser(const QString &user);
    
    const QString GetPassword() const;
    void SetPassword(const QString& password);
    
    const bool GetSavePassword() const;
    void SetSavePassword(bool save);

    const bool GetOnlyView() const;
    void SetOnlyView(bool only);
    
    const bool GetLocalCursor() const;
    void SetLocalCursor(bool cursor);
    
    const bool GetClipboard() const;
    void SetClipboard(bool c);
    
    enum class emProxy {
        No,
        SocksV4,
        SocksV5,
        Http,
        User = 100
    };
    
    const emProxy GetProxyType() const;
    void SetProxyType(emProxy type);
    const QString GetProxyHost() const;
    void SetProxyHost(const QString& host);
    const quint16 GetProxyPort() const;
    void SetProxyPort(quint16 port);
    const QString GetProxyUser() const;
    void SetProxyUser(const QString& user);
    const QString GetProxyPassword() const;
    void SetProxyPassword(const QString& password);
    
Q_SIGNALS:
    void sigUpdate();

protected:
    QByteArray PasswordSum(const std::string &password);
    int LoadPassword(const QString &szTitle, const QString &szKey, QString &password, QSettings &set);
    int SavePassword(const QString &szKey, const QString &password, QSettings &set, bool bSave = false);

private:
    bool m_bComplete;
    QString m_szName;
    QString m_szHost;
    quint16 m_nPort;
    
    QString m_szUser;
    QString m_szPassword;
    bool m_bSavePassword;
    
    bool m_bOnlyView;
    bool m_bLocalCursor;
    bool m_bClipboard;
    
    emProxy m_eProxyType;
    QString m_szProxyHost;
    quint16 m_nProxyPort;
    QString m_szProxyUser;
    QString m_szProxyPassword;
};

#endif // CPARAMTER_H
