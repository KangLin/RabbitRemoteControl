// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterSSH.h"

/*!
 * \brief The proxy parameters. Its UI is CParameterProxyUI
 *
 * \~
 * \see CParameterProxyUI
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class PLUGIN_EXPORT CParameterProxy : public CParameterOperate
{
    Q_OBJECT
public:
    explicit CParameterProxy(CParameterOperate *parent = nullptr,
                             const QString& szPrefix = QString());

    enum class TYPE{
        None = 0,
        System,
        SocksV5,
        Http,
        SSHTunnel,
    };
    Q_ENUM(TYPE)
    QList<TYPE> GetType() const;
    int SetType(QList<TYPE> type);
    TYPE GetUsedType() const;
    int SetUsedType(TYPE type);
    
    CParameterNet m_Http;
    CParameterNet m_SocksV5;
    CParameterSSHTunnel m_SSH;
    
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
