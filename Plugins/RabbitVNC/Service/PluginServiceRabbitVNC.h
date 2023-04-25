// Author: Kang Lin <kl222@126.com>

#ifndef CPLUGINSERVICETIGERVNC_H
#define CPLUGINSERVICETIGERVNC_H

#include "PluginServiceThread.h"
#include <QTranslator>

class CPluginServiceRabbitVNC : public CPluginServiceThread
{
    Q_OBJECT
    
    Q_INTERFACES(CPluginService)

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_PLUGIN_METADATA(IID CPluginService_iid)
#endif
    
public:
    explicit CPluginServiceRabbitVNC(QObject *parent = nullptr);
    virtual ~CPluginServiceRabbitVNC();

    // CPluginService interface
public:
    virtual const QString Protocol() const override;
    virtual const QString Name() const override;
    virtual const QString DisplayName() const override;
    virtual const QString Description() const override;

protected:
    virtual CService *NewService() override;

private:
    QTranslator m_Translator;
};

#endif // CPLUGINSERVICETIGERVNC_H
