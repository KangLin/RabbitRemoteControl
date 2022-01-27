#ifndef CPARAMETERFREERDP_H
#define CPARAMETERFREERDP_H

#include "Parameter.h"
#include "freerdp/freerdp.h"

class CParameterFreeRDP : public CParameter
{
    Q_OBJECT
    Q_PROPERTY(UINT ReconnectInterval READ GetReconnectInterval WRITE SetReconnectInterval NOTIFY sigReconnectIntervalChanged)
    
public:
    explicit CParameterFreeRDP(QObject *parent = nullptr);

    // CParameter interface
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;
    
    rdpSettings* m_pSettings;
    
    UINT GetReconnectInterval() const;
    void SetReconnectInterval(UINT newReconnectInterval);
    
signals:
    void sigReconnectIntervalChanged();
    
private:
    UINT m_nReconnectInterval; // Unit: second
};

#endif // CPARAMETERFREERDP_H
