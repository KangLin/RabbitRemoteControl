#ifndef CPARAMETERTIGERVNC_H
#define CPARAMETERTIGERVNC_H

#include "ParameterConnecter.h"

class CParameterTigerVnc : public CParameterConnecter
{
    Q_OBJECT
public:
    explicit CParameterTigerVnc(QObject *parent = nullptr);
    
public:
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;
    
    /*!
     * \brief Check whether the parameters are complete
     *  to decide whether to open the parameter dialog 
     * \return 
     */
    virtual bool GetCheckCompleted();
    
    enum COLOR_LEVEL {
        Full,
        Medium,
        Low,
        VeryLow
    };

    bool GetShared() const;
    void SetShared(bool newShared);
    
    bool GetBufferEndRefresh() const;
    void SetBufferEndRefresh(bool newBufferEndRefresh);
    
    bool GetSupportsDesktopResize() const;
    void SetSupportsDesktopResize(bool newSupportsDesktopResize);
    
    bool GetAutoSelect() const;
    void SetAutoSelect(bool newAutoSelect);
    
    COLOR_LEVEL GetColorLevel() const;
    void SetColorLevel(COLOR_LEVEL newColorLevel);
    
    int GetEncoding() const;
    void SetEncoding(int newEncoding);
    
    bool GetEnableCompressLevel() const;
    void SetEnableCompressLevel(bool newCompressLevel);
    
    int GetCompressLevel() const;
    void SetCompressLevel(int newCompressLevel);
    
    bool GetNoJpeg() const;
    void SetNoJpeg(bool newNoJpeg);
    
    int GetQualityLevel() const;
    void SetQualityLevel(int newQualityLevel);
    
    bool GetIce() const;
    void SetIce(bool newIce);
    
    const QString &GetSignalServer() const;
    void SetSignalServer(const QString &newSignalServer);
    
    quint16 GetSignalPort() const;
    void SetSignalPort(quint16 newSignalPort);
    
    const QString &GetSignalUser() const;
    void SetSignalUser(const QString &newSignalUser);
    
    const QString &GetSignalPassword() const;
    void SetSignalPassword(const QString &newSignalPassword);
    
    const QString &GetPeerUser() const;
    void SetPeerUser(const QString &newPeerUser);
    
    const QString &GetStunServer() const;
    void SetStunServer(const QString &newStunServer);
    
    quint16 GetStunPort() const;
    void SetStunPort(quint16 newStunPort);
    
    const QString &GetTurnServer() const;
    void SetTurnServer(const QString &newTurnServer);
    
    quint16 GetTurnPort() const;
    void SetTurnPort(quint16 newTurnPort);
    
    const QString &GetTurnUser() const;
    void SetTurnUser(const QString &newTurnUser);
    
    const QString &GetTurnPassword() const;
    void SetTurnPassword(const QString &newTurnPassword);
    
private:
    QString szServerName;

    bool m_bShared;
    bool m_bBufferEndRefresh;
    bool m_bSupportsDesktopResize;

    bool m_bAutoSelect;
    COLOR_LEVEL m_nColorLevel;
    int m_nEncoding;
    bool m_bCompressLevel;
    int m_nCompressLevel;
    bool m_bNoJpeg;
    int m_nQualityLevel;
    
    bool m_bIce;
    QString m_szSignalServer;
    quint16 m_nSignalPort;
    QString m_szSignalUser, m_szSignalPassword, m_szPeerUser;
    QString m_szStunServer;
    quint16 m_nStunPort;
    QString m_szTurnServer;
    quint16 m_nTurnPort;
    QString m_szTurnUser, m_szTurnPassword;
    
};

#endif // CPARAMETERTIGERVNC_H
