#ifndef CPARAMETERPROXY_H
#define CPARAMETERPROXY_H

#include "ParameterNet.h"

/*!
 * \brief The proxy parameters. Its UI is CParameterProxyUI
 *
 * \~
 * \see CParameterProxyUI
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class CLIENT_EXPORT CParameterProxy : public CParameterConnecter
{
    Q_OBJECT
public:
    explicit CParameterProxy(CParameterConnecter *parent = nullptr,
                             const QString& szPrefix = QString());

    enum class TYPE{
        None = 0,
        Default,
        SockesV5,
        SSHTunnel,
    };
    Q_ENUM(TYPE)
    QList<TYPE> GetType() const;
    int SetType(QList<TYPE> type);
    TYPE GetUsedType() const;
    int SetUsedType(TYPE type);

    CParameterNet m_SockesV5;
    CParameterNet m_SSH;
    
    int SetTypeName(TYPE t, const QString& szName);
    QString ConvertTypeToName(TYPE t);

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

private:
    QList<TYPE> m_Type;
    TYPE m_UsedType;

    QMap<TYPE, QString> m_TypeName;
};

#endif // CPARAMETERPROXY_H
