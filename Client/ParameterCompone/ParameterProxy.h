#ifndef CPARAMETERPROXY_H
#define CPARAMETERPROXY_H

#include "ParameterNet.h"

/*!
 * \brief The proxy parameters
 *
 * \~
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class CLIENT_EXPORT CParameterProxy : public CParameterConnecter
{
    Q_OBJECT
public:
    explicit CParameterProxy(CParameterConnecter *parent = nullptr,
                             const QString& szPrefix = QString());

    enum class TYPE{
        None = 0x01,
        SockesV5 = 0x02,
        SSHTunnel = 0x04,
        Default = 0x08
    };
    TYPE GetType() const;
    int SetType(TYPE type);

    CParameterNet m_Sockes;
    CParameterNet m_SSH;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

private:
    TYPE m_Type;
};

#endif // CPARAMETERPROXY_H
