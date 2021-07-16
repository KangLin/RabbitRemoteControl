#ifndef CSERVICE_H_KL_2021_07_13
#define CSERVICE_H_KL_2021_07_13

#pragma once

#include <QObject>

class CService : public QObject
{
    Q_OBJECT
public:
    explicit CService(QObject *parent = nullptr);
    
    virtual bool Enable();
    virtual int Init();
    virtual int Clean();
    
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
};

#endif // CSERVICE_H_KL_2021_07_13
