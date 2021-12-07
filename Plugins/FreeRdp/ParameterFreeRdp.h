#ifndef CPARAMETERFREERDP_H
#define CPARAMETERFREERDP_H

#include "Parameter.h"
#include "freerdp/freerdp.h"

class CParameterFreeRdp : public CParameter
{
    Q_OBJECT
    Q_PROPERTY(UINT ReconnectInterval READ GetReconnectInterval WRITE SetReconnectInterval NOTIFY sigReconnectIntervalChanged)
    
public:
    explicit CParameterFreeRdp(QObject *parent = nullptr);

    // CParameter interface
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;
    
    rdpSettings* m_pSettings;
    
    UINT GetReconnectInterval() const;
    void SetReconnectInterval(UINT newReconnectInterval);
    
signals:
    void sigReconnectIntervalChanged();
    
private:
    UINT m_nReconnectInterval; // Unit: second
};

#endif // CPARAMETERFREERDP_H
