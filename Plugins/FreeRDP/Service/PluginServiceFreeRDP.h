// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINSERVICEFREERDP_H
#define CPLUGINSERVICEFREERDP_H

#include "PluginService.h"

class CPluginServiceFreeRDP : public CPluginService
{
    Q_OBJECT
    
    Q_INTERFACES(CPluginService)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginService_iid)
#endif
    
public:
    explicit CPluginServiceFreeRDP(QObject *parent = nullptr);
    virtual ~CPluginServiceFreeRDP();

    // CPluginService interface
public:
    virtual const QString Protocol() const override;
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    
protected:
    virtual CService *NewService() override;
};

#endif // CPLUGINSERVICEFREERDP_H
