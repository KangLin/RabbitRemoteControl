// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Operate.h"

class CBackend;
class CBackendThread;
class CFrmViewerTemplateServer;
class CParameterTemplateServer;
class COperateTemplateServer : public COperate
{
    Q_OBJECT
public:
    COperateTemplateServer(CPlugin *plugin);
    virtual ~COperateTemplateServer();

    /*!
     * \brief Get parameter
     */
    [[nodiscard]] virtual CParameterTemplateServer* GetParameter() const;

    // COperate interface
public:
    virtual const qint16 Version() const override;
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;

protected:
    virtual int SetPluginParameters(CParameterPlugin *pPara) override;
    Q_INVOKABLE virtual CBackend* InstanceBackend();
    Q_INVOKABLE virtual int Initial() override;
    Q_INVOKABLE virtual int Clean() override;
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
    CParameterTemplateServer* m_pPara;
    CFrmViewerTemplateServer* m_pViewer;
    CBackendThread* m_pThread;
};
