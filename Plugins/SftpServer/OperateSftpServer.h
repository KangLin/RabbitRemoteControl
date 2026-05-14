// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Operate.h"

class CBackend;
class CBackendThread;
class CFrmViewerSftpServer;
class CParameterSftpServer;
class COperateSftpServer : public COperate
{
    Q_OBJECT
public:
    COperateSftpServer(CPlugin *plugin);
    virtual ~COperateSftpServer();

    /*!
     * \brief Get parameter
     */
    [[nodiscard]] virtual CParameterSftpServer* GetParameter() const;

    // COperate interface
public:
    virtual const qint16 Version() const override;
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;

protected:
    virtual int SetPluginParameters(CParameterPlugin *pPara) override;
    Q_INVOKABLE virtual CBackend* InstanceBackend();
    virtual int Initial() override;
    virtual int Clean() override;
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
    CParameterSftpServer* m_pPara;
    CFrmViewerSftpServer* m_pViewer;
    CBackendThread* m_pThread;

};
