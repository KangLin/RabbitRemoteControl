#ifndef CPluginLibVNCServer_H_KL_2021_07_23
#define CPluginLibVNCServer_H_KL_2021_07_23

#include "Plugin.h"
#include <QTranslator>

class CPluginLibVNCServer : public CPlugin
{
    Q_OBJECT
    Q_INTERFACES(CPlugin)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPlugin_iid)
#endif
    
public:
    CPluginLibVNCServer(QObject *parent = nullptr);
    virtual ~CPluginLibVNCServer() override;
    
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

#endif // CPluginLibVNCServer_H_KL_2021_07_23
