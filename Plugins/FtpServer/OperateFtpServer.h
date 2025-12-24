// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#pragma once
#include "Operate.h"
#include "BackendThread.h"
#include "ParameterFtpServer.h"
#include "FrmMain.h"

class COperateFtpServer : public COperate
{
    Q_OBJECT

public:
    explicit COperateFtpServer(CPlugin *plugin);
    ~COperateFtpServer();

public:
    virtual const qint16 Version() const override;
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;
    Q_INVOKABLE virtual CBackend* InstanceBackend();
    QSharedPointer<CParameterFtpServer> GetParameter();

protected:
    virtual int SetGlobalParameters(CParameterPlugin *pPara) override;
    virtual int Initial() override;
    virtual int Clean() override;
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

private Q_SLOTS:
    void slotStart(bool checked);

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
    CBackendThread* m_pThread;
    QSharedPointer<CParameterFtpServer> m_Para;
    CFrmMain* m_pView;
    QAction* m_pStart;
    
    friend class CFrmMain;
};
