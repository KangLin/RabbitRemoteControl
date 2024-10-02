// Author: Kang Lin <kl222@126.com>

#ifndef PARAMETERWAKEONLAN_H
#define PARAMETERWAKEONLAN_H

#include "ParameterConnecter.h"

/*!
 * \brief The wake on lan parameters. it's UI is CParameterWakeOnLanUI
 * \see CParameterWakeOnLanUI
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class CLIENT_EXPORT CParameterWakeOnLan : public CParameterConnecter
{
    Q_OBJECT
public:
    explicit CParameterWakeOnLan(CParameterConnecter* parent,
                                 const QString& szPrefix = QString());

    const bool GetEnable() const;
    int SetEnable(bool bEnable);

    const QString GetMac() const;
    int SetMac(const QString &szMac);

    const QString GetBroadcastAddress() const;
    int SetBroadcastAddress(const QString& szBroadcastAddress);

    const QString GetNetworkInterface() const;
    int SetNetworkInterface(const QString& szName);

    const quint16 GetPort() const;
    int SetPort(quint16 port);

    const QString GetPassword() const;
    int SetPassword(const QString& szPassword);

    const bool GetSavePassword() const;
    /*!
     * \brief Set save password
     * \param save
     */
    int SetSavePassword(bool save);

    const int GetRepeat() const;
    int SetRepeat(int nRepeat);

    //! Unit: ms
    const int GetInterval() const;
    //! Unit: ms
    int SetInterval(int nInterval);

    //! Unit: s
    const int GetDelay() const;
    //! Unit: s
    int SetDelay(int nDelay);

private:
    bool m_bEnable;
    QString m_szMac;
    QString m_szBoardcastAddress;
    QString m_szNetworkInteface;
    quint16 m_nPort;
    QString m_szPassword;
    bool m_bSavePassword;

    int m_nRepeat;
    int m_nInterval; //! Unit: ms
    int m_nDelay;    //! Unit: s

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
    virtual bool OnCheckValidity() override;

    // CParameterConnecter interface
protected slots:
    virtual void slotSetParameterClient() override;
};

#endif // PARAMETERWAKEONLAN_H
