// Author: Kang Lin <kl222@126.com>

#pragma once

#include "OperateServer.h"

class CBackend;
class CParameterSftpServer;
class COperateSftpServer : public COperateServer
{
    Q_OBJECT
public:
    explicit COperateSftpServer(CPlugin *plugin);
    virtual ~COperateSftpServer();

    /*!
     * \brief Get parameter
     */
    [[nodiscard]] virtual CParameterSftpServer* GetParameter() const;

    // COperate interface
public:
    virtual const QString Name() override;
    virtual const QString Description() override;
    virtual const qint16 Version() const override;

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
};
