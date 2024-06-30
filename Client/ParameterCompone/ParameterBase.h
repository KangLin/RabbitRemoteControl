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

public:
    explicit CParameterBase(QObject* parent = nullptr);
    explicit CParameterBase(CParameterConnecter* parent,
                            const QString& szPrefix = QString());

    const QString GetName() const;
    void SetName(const QString& szName);

    CParameterNet m_Net;

private:
    QString m_szName;

    // CParameter interface
protected:
    virtual int onLoad(QSettings &set) override;
    virtual int onSave(QSettings &set) override;
};

#endif // CPARAMETERBASE_H
