#ifndef CSERVICE_H_KL_2021_07_13
#define CSERVICE_H_KL_2021_07_13

#pragma once

#include <QSharedPointer>
#include <QDataStream>
#include "ParameterService.h"

class CPluginService;

///
/// \~chinese 服务接口。由具体的协议实现。
/// 
/// \~english
/// \brief The service interface. by specific protocol implement
/// 
/// \~
/// \ingroup LIBAPI_SERVICE
/// 
class SERVICE_EXPORT CService : public QObject
{
    Q_OBJECT
public:
    explicit CService(CPluginService *plugin);
    virtual ~CService();

    virtual bool Enable();
    
    virtual int Init();
    virtual int Clean();

    CParameterService* GetParameters();
    
    virtual int SaveConfigure(const QString& szDir = QString());

protected Q_SLOTS:
    virtual void slotProcess();

protected:
    virtual int OnInit();
    virtual int OnClean();
    /// \return
    /// \li -1: error
    /// \li  0: Success
    /// \li >0: Stop call slotProcess
    virtual int OnProcess();

    CParameterService* m_pPara;
    CPluginService* m_pPlugin;
};

#endif // CSERVICE_H_KL_2021_07_13
