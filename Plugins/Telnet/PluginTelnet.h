#ifndef CPLUGINTELNET_H_KL_2021_07_23
#define CPLUGINTELNET_H_KL_2021_07_23

#pragma once

#include "Plugin.h"
#include <QTranslator>

class CPluginTelnet : public CPlugin
{
    Q_OBJECT
public:
    explicit CPluginTelnet(QObject *parent = nullptr);
    virtual ~CPluginTelnet();
    
    Q_INTERFACES(CPlugin)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPlugin_iid)
#endif

    // CPluginFactory interface
public:
    virtual const QString Protol() const override;
    virtual const QString Name() const override;
    virtual const QString Description() const override;
    virtual const QIcon Icon() const override;
    virtual CConnecter *CreateConnecter(const QString &szProtol) override;
    
private:
    QTranslator m_Translator;
    
};

#endif // CPLUGINTELNET_H_KL_2021_07_23
