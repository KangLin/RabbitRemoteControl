#ifndef CPARAMETERFREERDP_H
#define CPARAMETERFREERDP_H

#include "ParameterBase.h"
#include "ParameterProxy.h"
#include "freerdp/freerdp.h"

//! [Declare CParameterFreeRDP]
class CParameterFreeRDP : public CParameterBase
//! [Declare CParameterFreeRDP]
{
    Q_OBJECT
    Q_PROPERTY(bool RedirectionPrinter READ GetRedirectionPrinter WRITE SetRedirectionPrinter NOTIFY sigRedirectionPrinterChanged)
    Q_PROPERTY(UINT ReconnectInterval READ GetReconnectInterval WRITE SetReconnectInterval NOTIFY sigReconnectIntervalChanged)
    Q_PROPERTY(RedirecionSoundType RedirectionSound READ GetRedirectionSound WRITE SetRedirectionSound NOTIFY sigRedirectionSoundChanged)
    Q_PROPERTY(QString RedirectionSoundParameters READ GetRedirectionSoundParameters WRITE SetRedirectionSoundParameters NOTIFY sigRedirectionSoundParametersChanged)
    Q_PROPERTY(bool RedirectionMicrophone READ GetRedirectionMicrophone WRITE SetRedirectionMicrophone NOTIFY sigRedirectionMicrophoneChanged)
    Q_PROPERTY(QStringList RedirectionDrives READ GetRedirectionDrives WRITE SetRedirectionDrives NOTIFY sigRedirectionDrivesChanged)

public:
    explicit CParameterFreeRDP(QObject *parent = nullptr);

    // CParameter interface
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
    
public:

    void SetDomain(const QString& szDomain);
    const QString GetDomain() const;
    
    UINT32 GetDesktopWidth() const;
    int SetDesktopWidth(UINT32 nWidth);
    
    UINT32 GetDesktopHeight() const;
    int SetDesktopHeight(UINT32 nHeight);
    
    UINT32 GetColorDepth() const;
    int SetColorDepth(UINT32 color);
    
    bool GetUseMultimon() const;
    int SetUseMultimon(bool bUse);

    UINT GetReconnectInterval() const;
    void SetReconnectInterval(UINT newReconnectInterval);
    
    bool GetShowVerifyDiaglog() const;
    void SetShowVerifyDiaglog(bool bShow);
    
    enum class RedirecionSoundType {
        Disable,
        Local,
        Remote
    };
    RedirecionSoundType GetRedirectionSound() const;
    void SetRedirectionSound(RedirecionSoundType newRedirectionSound);
    bool GetRedirectionMicrophone() const;
    void SetRedirectionMicrophone(bool newRedirectionMicrophone);
    QStringList GetRedirectionDrives() const;
    void SetRedirectionDrives(const QStringList &newRedirectionDrive);
    
    bool GetRedirectionPrinter() const;
    void SetRedirectionPrinter(bool newRedirectionPrinter);
    
    const QString &GetRedirectionSoundParameters() const;
    void SetRedirectionSoundParameters(const QString &newRedirectionSoundParameters);
    
    const QString &GetRedirectionMicrophoneParameters() const;
    void SetRedirectionMicrophoneParameters(const QString &newRedirectionMicrophoneParameters);
    
    CParameterProxy m_Proxy;

signals:
    void sigReconnectIntervalChanged();
    
    void sigRedirectionSoundChanged(RedirecionSoundType RedirectionSound);
    void sigRedirectionSoundParametersChanged();
    void sigRedirectionMicrophoneChanged(bool RedirectionMicrophone);
    void sigRedirectionMicrophoneParametersChanged();
    void sigRedirectionDrivesChanged(QStringList RedirectionDrive);
    void sigRedirectionPrinterChanged(bool RedirectionPrinter);
    
private:
    QString m_szDomain;
    
    UINT32 m_nWidth;
    UINT32 m_nHeight;
    
    UINT32 m_nColorDepth;
    bool m_bUseMultimon;
    
    UINT32 m_nReconnectInterval; // Unit: second
    bool m_bShowVerifyDiaglog;
    
    bool m_bRedirectionPrinter;
    RedirecionSoundType m_nRedirectionSound;
    QString m_szRedirectionSoundParameters;
    bool m_bRedirectionMicrophone;
    QString m_szRedirectionMicrophoneParameters;
    QStringList m_lstRedirectionDrives;
    Q_PROPERTY(QString RedirectionMicrophoneParameters READ GetRedirectionMicrophoneParameters WRITE SetRedirectionMicrophoneParameters NOTIFY sigRedirectionMicrophoneParametersChanged)
    
};

#endif // CPARAMETERFREERDP_H
