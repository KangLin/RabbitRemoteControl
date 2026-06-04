// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateServer.h"
class CBackend;
class CParameterTemplateServer;
class COperateTemplateServer : public COperateServer
{
    Q_OBJECT
public:
    explicit COperateTemplateServer(CPlugin *plugin);
    virtual ~COperateTemplateServer();

    /*!
     * \brief Get parameter
     */
    [[nodiscard]] virtual CParameterTemplateServer* GetParameter() const;

public:
    virtual const qint16 Version() const override;

protected:
    virtual int SetPluginParameters(CParameterPlugin *pPara) override;
    Q_INVOKABLE virtual CBackend* InstanceBackend() override;
    Q_INVOKABLE virtual int Initial() override;
    Q_INVOKABLE virtual int Clean() override;
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
    CParameterTemplateServer* m_pPara;
};
