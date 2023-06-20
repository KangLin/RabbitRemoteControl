// Author: Kang Lin <kl222@126.com>

#ifndef CPluginRabbitVnc_H_KL_2021_07_23
#define CPluginRabbitVnc_H_KL_2021_07_23

#pragma once

#include "PluginClientThread.h"
#include <QTranslator>

class CPluginRabbitVNC : public CPluginClientThread
{
    Q_OBJECT
    
    /// ![Qt plugin interface]
    Q_INTERFACES(CPluginClient)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginClient_iid)
#endif
    /// ![Qt plugin interface]
    
public:
    explicit CPluginRabbitVNC(QObject *parent = nullptr);
    virtual ~CPluginRabbitVNC() override;
    
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    virtual const QString Protocol() const override;
    virtual const QIcon Icon() const override;
    virtual const QString Details() const override;

protected:
    virtual CConnecterDesktop* OnCreateConnecter(const QString& szProtocol) override;    
};

#endif // CPluginRabbitVnc_H_KL_2021_07_23
