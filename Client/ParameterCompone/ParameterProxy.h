#ifndef CPARAMETERPROXY_H
#define CPARAMETERPROXY_H

#include <ParameterNet.h>

class CLIENT_EXPORT CParameterProxy : public CParameterConnecter
{
    Q_OBJECT
public:
    explicit CParameterProxy(CParameterConnecter *parent = nullptr,
                             const QString& szPrefix = QString());

    enum class TYPE{
        No,
        Socket5
    };
    TYPE GetType() const;
    int SetType(TYPE type);

    CParameterNet m_Socket5;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

private:
    TYPE m_Type;
};

#endif // CPARAMETERPROXY_H
