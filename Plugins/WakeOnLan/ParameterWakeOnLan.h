// Author: Kang Lin <kl222@126.com>

#ifndef PARAMETERWAKEONLAN_H
#define PARAMETERWAKEONLAN_H

#include "ParameterConnecter.h"
#include "ParameterNet.h"

/*!
 * \brief The wake on lan parameters. it's UI is CParameterWakeOnLanUI
 * \see CParameterWakeOnLanUI
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class CParameterWakeOnLan : public CParameterConnecter
{
    Q_OBJECT
public:
    explicit CParameterWakeOnLan(QObject* parent = nullptr);

    const bool GetEnable() const;
    int SetEnable(bool bEnable);

    // Target IP
    CParameterNet m_Net;

    const QString GetMac() const;
    int SetMac(const QString &szMac);

    const QString GetBroadcastAddress() const;
    int SetBroadcastAddress(const QString& szBroadcastAddress);

    /*!
     * \brief Get network interface.
     * \return Source IP
     */
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

    //! Unit: ms
    const int GetTimeOut() const;
    //! Unit: ms
    int SetTimeOut(int nTimeout);

    enum class HostState{
        Online,
        GetMac,
        WakeOnLan,
        Offline
    };

    HostState GetHostState() const;
    void SetHostState(HostState newHostState);

signals:
    void sigHostStateChanged();

private:
    bool m_bEnable;
    HostState m_HostState;
    QString m_szMac;
    QString m_szBoardcastAddress;
    QString m_szNetworkInteface;
    quint16 m_nPort;
    QString m_szPassword;
    bool m_bSavePassword;

    int m_nRepeat;
    int m_nInterval; //! Unit: ms
    int m_nTimeOut;    //! Unit: ms

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
