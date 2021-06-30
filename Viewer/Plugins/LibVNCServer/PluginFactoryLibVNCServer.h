#ifndef CPLUGINFACTORYLIBVNC_H
#define CPLUGINFACTORYLIBVNC_H

#include "PluginFactory.h"
#include <QTranslator>

class CPluginFactoryLibVNCServer : public CPluginFactory
{
    Q_OBJECT
    Q_INTERFACES(CPluginFactory)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginFactory_iid)
#endif
    
public:
    CPluginFactoryLibVNCServer(QObject *parent = nullptr);
    virtual ~CPluginFactoryLibVNCServer() override;
    
    // CPluginFactory interface
public:
    virtual const QString Name() const override;
    virtual const QString Description() const override;
    virtual const QString Protol() const override;
    virtual const QIcon Icon() const override;
    virtual CConnecter *CreateConnecter(const QString &szProtol) override;
    
private:
    QTranslator m_Translator;
};

#endif // CPLUGINFACTORYLIBVNC_H
