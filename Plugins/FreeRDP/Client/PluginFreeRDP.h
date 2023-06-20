// Author: Kang Lin <kl222@126.com>

#ifndef CPluginFreeRdp_H_KL_2021_07_23
#define CPluginFreeRdp_H_KL_2021_07_23

#pragma once

#include "PluginClient.h"

class CPluginFreeRDP : public CPluginClient
{
    Q_OBJECT
    
    // [Qt plugin interface]
    Q_INTERFACES(CPluginClient)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginClient_iid)
#endif
    // [Qt plugin interface]
    
public:
    explicit CPluginFreeRDP(QObject *parent = nullptr);
    virtual ~CPluginFreeRDP() override;
    
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    virtual const QString Protocol() const override;
    virtual const QIcon Icon() const override;
    virtual const QString Details() const override;
    
private:
    virtual CConnecter* CreateConnecter(const QString& szProtocol) override;
    
private:
    QTranslator m_Translator;
};

#endif // CPluginFreeRdp_H_KL_2021_07_23
