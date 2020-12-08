#ifndef MANAGECONNECTFREERDP_H
#define MANAGECONNECTFREERDP_H

#include "PluginFactory.h"
#include <QTranslator>

class CPluginFactoryFreeRdp : public CPluginFactory
{
    Q_OBJECT
    Q_INTERFACES(CPluginFactory)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginFactory_iid)
#endif
    
public:
    explicit CPluginFactoryFreeRdp(QObject *parent = nullptr);
    virtual ~CPluginFactoryFreeRdp() override;
    
    virtual QString Name() override;
    virtual QString Description() override;
    virtual QString Protol() override;
    virtual QIcon Icon() override;
    
    virtual CConnecter* CreateConnecter(const QString& szProtol) override;
    
private:
    QTranslator m_Translator;
};

#endif // MANAGECONNECTFREERDP_H
