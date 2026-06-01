// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateServer.h"
class CBackend;
class CParameterFtpServer;
class COperateFtpServer : public COperateServer
{
    Q_OBJECT
public:
    explicit COperateFtpServer(CPlugin *plugin);
    virtual ~COperateFtpServer();

    /*!
     * \brief Get parameter
     */
    [[nodiscard]] virtual CParameterFtpServer* GetParameter() const;

    // COperate interface
public:
    virtual const QString Name() override;
    virtual const QString Description() override;
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
    CParameterFtpServer* m_pPara;
};
