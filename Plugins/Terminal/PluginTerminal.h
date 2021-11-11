#ifndef CPLUGINTERMINAL_H_KL_2021_07_23
#define CPLUGINTERMINAL_H_KL_2021_07_23

#include "PluginViewer.h"
#include <QTranslator>

class CPluginTerminal : public CPluginViewer
{
    Q_OBJECT
    Q_INTERFACES(CPluginViewer)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginViewer_iid)
#endif
    
public:
    explicit CPluginTerminal(QObject *parent = nullptr);
    virtual ~CPluginTerminal();
    
    virtual const QString Protol() const override;
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;
    virtual const QIcon Icon() const override;
    virtual CConnecter *CreateConnecter(const QString &szProtol) override;
};

#endif // CPLUGINTERMINAL_H_KL_2021_07_23
