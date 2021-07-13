#ifndef CPLUGINSERVICETIGERVNC_H
#define CPLUGINSERVICETIGERVNC_H

#include "PluginService.h"
#include <QTranslator>

class CPluginServiceTigerVNC : public CPluginService
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
    virtual const QString Protol() const override;
    virtual const QString Name() const override;
    virtual const QString Description() const override;
    
protected:
    virtual CService *NewService() override;
    
private:
    QTranslator m_Translator;
};

#endif // CPLUGINSERVICETIGERVNC_H
