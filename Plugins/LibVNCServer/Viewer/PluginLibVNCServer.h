#ifndef CPluginLibVNCServer_H_KL_2021_07_23
#define CPluginLibVNCServer_H_KL_2021_07_23

#include "PluginViewer.h"
#include <QTranslator>

class CPluginLibVNCServer : public CPluginViewer
{
    Q_OBJECT
    Q_INTERFACES(CPluginViewer)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginViewer_iid)
#endif
    
public:
    CPluginLibVNCServer(QObject *parent = nullptr);
    virtual ~CPluginLibVNCServer() override;
    
    // CPluginFactory interface
public:
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    virtual const QString Protol() const override;
    virtual const QIcon Icon() const override;
    virtual CConnecter *CreateConnecter(const QString &szProtol) override;
};

#endif // CPluginLibVNCServer_H_KL_2021_07_23
