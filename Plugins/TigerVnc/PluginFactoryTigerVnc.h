// Author: Kang Lin <kl222@126.com>

#ifndef CMANAGECONNECTTIGERVNC_H
#define CMANAGECONNECTTIGERVNC_H

#include "Plugin.h"
#include <QTranslator>

class CPluginFactoryTigerVnc : public CPlugin
{
    Q_OBJECT
    
    /// ![Qt plugin interface]
    Q_INTERFACES(CPlugin)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPlugin_iid)
#endif
    /// ![Qt plugin interface]
    
public:
    explicit CPluginFactoryTigerVnc(QObject *parent = nullptr);
    virtual ~CPluginFactoryTigerVnc() override;
    
    virtual const QString Name() const override;
    virtual const QString Description() const override;
    virtual const QString Protol() const override;
    virtual const QIcon Icon() const override;
    virtual CConnecter* CreateConnecter(const QString& szProtol) override;
    
private:
    QTranslator m_Translator;
};

#endif // CMANAGECONNECTTIGERVNC_H
