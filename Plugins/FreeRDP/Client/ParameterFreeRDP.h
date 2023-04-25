#ifndef CPARAMETERFREERDP_H
#define CPARAMETERFREERDP_H

#include "ParameterConnecter.h"
#include "freerdp/freerdp.h"

class CParameterFreeRDP : public CParameterConnecter
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
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;
    
    rdpSettings* m_pSettings;
    
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
    
signals:
    void sigReconnectIntervalChanged();
    
    void sigRedirectionSoundChanged(RedirecionSoundType RedirectionSound);
    void sigRedirectionSoundParametersChanged();
    void sigRedirectionMicrophoneChanged(bool RedirectionMicrophone);
    void sigRedirectionMicrophoneParametersChanged();
    void sigRedirectionDrivesChanged(QStringList RedirectionDrive);
    void sigRedirectionPrinterChanged(bool RedirectionPrinter);

private:
    UINT m_nReconnectInterval; // Unit: second
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
