#ifndef CPluginLibVNCServer_H_KL_2021_07_23
#define CPluginLibVNCServer_H_KL_2021_07_23

#include "PluginClient.h"
#include <QTranslator>

class CPluginLibVNCServer : public CPluginClient
{
    Q_OBJECT
    Q_INTERFACES(CPluginClient)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginClient_iid)
#endif
    
public:
    CPluginLibVNCServer(QObject *parent = nullptr);
    virtual ~CPluginLibVNCServer() override;
    
    // CPluginFactory interface
public:
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    virtual const QString Protocol() const override;
    virtual const QIcon Icon() const override;
private:
    virtual CConnecter *CreateConnecter(const QString &szProtocol) override;
};

#endif // CPluginLibVNCServer_H_KL_2021_07_23
