// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateServer.h"
class CBackend;
class CParameterFreeRDPServer;
class COperateFreeRDPServer : public COperateServer
{
    Q_OBJECT
public:
    explicit COperateFreeRDPServer(CPlugin *plugin);
    virtual ~COperateFreeRDPServer();

    /*!
     * \brief Get parameter
     */
    [[nodiscard]] virtual CParameterFreeRDPServer* GetParameter() const;

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
    CParameterFreeRDPServer* m_pPara;
};
