// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QTcpSocket>
#include "OperateTerminal.h"
#include "ParameterRawStream.h"
#include "Stats.h"

class COperateRawStream : public COperateTerminal
{
    Q_OBJECT
public:
    explicit COperateRawStream(CPlugin *plugin);
    virtual ~COperateRawStream();

    [[nodiscard]] const QString Id() override;
    [[nodiscard]] const QString Name() override;
    [[nodiscard]] const QString Description() override;

    virtual int Start() override;
    virtual int Stop() override;

    // COperate interface
private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

    // COperateTerminal interface
public:
    virtual CBackend *InstanceBackend() override;

private Q_SLOTS:
    void slotReadyRead();

private:
    CParameterRawStream m_Parameter;
    
    QTcpSocket m_TcpSocket;
};

