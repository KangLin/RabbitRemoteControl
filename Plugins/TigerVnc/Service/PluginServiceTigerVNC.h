// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINSERVICETIGERVNC_H
#define CPLUGINSERVICETIGERVNC_H

#include "PluginServiceThread.h"
#include <QTranslator>

class CPluginServiceTigerVNC : public CPluginServiceThread
{
    Q_OBJECT
    
    Q_INTERFACES(CPluginService)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginService_iid)
#endif
    
public:
    explicit CPluginServiceTigerVNC(QObject *parent = nullptr);
    virtual ~CPluginServiceTigerVNC();
    
    // CPluginService interface
public:
    virtual const QString Protocol() const override;
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    
protected:
    virtual CService *NewService() override;
};

#endif // CPLUGINSERVICETIGERVNC_H
