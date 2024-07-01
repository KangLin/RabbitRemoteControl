#ifndef CPARAMETERBASE_H
#define CPARAMETERBASE_H

#include "ParameterNet.h"

/*!
 * \~chinese
 * \brief
 * 连接参数接口。包括基本参数（网络参数等）。
 * 所有插件的连接参数都应从此类派生。
 *
 * \~english
 * \brief
 *  The interface of connecter parameters.
 *  include base parameters(network etc).
 *
 *  All plug-in connection parameters should be derived from this class.
 *
 * \~
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class CLIENT_EXPORT CParameterBase : public CParameterConnecter
{
    Q_OBJECT
    Q_PROPERTY(QString Name READ GetName WRITE SetName NOTIFY sigNameChanged)
    Q_PROPERTY(QString ServerName READ GetServerName WRITE SetServerName)
    Q_PROPERTY(bool ShowServerName READ GetShowServerName WRITE SetShowServerName NOTIFY sigShowServerNameChanged)
    Q_PROPERTY(bool OnlyView READ GetOnlyView WRITE SetOnlyView)
    Q_PROPERTY(bool LocalCursor READ GetLocalCursor WRITE SetLocalCursor)

public:
    explicit CParameterBase(QObject* parent = nullptr);
    explicit CParameterBase(CParameterConnecter* parent,
                            const QString& szPrefix = QString());

    const QString GetName() const;
    void SetName(const QString& szName);

    const QString GetServerName() const;
    void SetServerName(const QString& szName);

    bool GetShowServerName() const;
    void SetShowServerName(bool NewShowServerName);

    CParameterNet m_Net;

    virtual const bool GetOnlyView() const;
    virtual void SetOnlyView(bool only);

    const bool GetLocalCursor() const;
    void SetLocalCursor(bool cursor);
    
    virtual const bool GetClipboard() const;
    virtual void SetClipboard(bool c);

Q_SIGNALS:
    void sigNameChanged(const QString &name = QString());
    void sigShowServerNameChanged();

private:
    int Init();

    QString m_szName;
    QString m_szServerName;
    bool m_bShowServerName;

    bool m_bOnlyView;
    bool m_bLocalCursor;
    bool m_bClipboard;
    
    // CParameter interface
protected:
    virtual int onLoad(QSettings &set) override;
    virtual int onSave(QSettings &set) override;
};

#endif // CPARAMETERBASE_H
