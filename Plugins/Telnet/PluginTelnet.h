#ifndef CPLUGINTELNET_H_KL_2021_07_23
#define CPLUGINTELNET_H_KL_2021_07_23

#pragma once

#include "PluginViewer.h"
#include <QTranslator>

class CPluginTelnet : public CPluginViewer
{
    Q_OBJECT
public:
    explicit CPluginTelnet(QObject *parent = nullptr);
    virtual ~CPluginTelnet();
    
    Q_INTERFACES(CPluginViewer)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginViewer_iid)
#endif

    // CPluginFactory interface
public:
    virtual const QString Protol() const override;
    virtual const QString Name() const override;
    virtual const QString Description() const override;
    virtual const QIcon Icon() const override;
private:
    virtual CConnecter *CreateConnecter(const QString &szProtol) override;
};

#endif // CPLUGINTELNET_H_KL_2021_07_23
