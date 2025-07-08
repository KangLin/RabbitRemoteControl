// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once
#include "Operate.h"
#include "FrmFileTransfer.h"
#include "ParameterFileTransfer.h"

class COperateFileTransfer : public COperate
{
    Q_OBJECT
public:
    explicit COperateFileTransfer(CPlugin *plugin);
    virtual ~COperateFileTransfer();

    virtual const QString Id() override;
    virtual const QString Name() override;
    virtual const QString Description() override;
    virtual const QString Protocol() const override;
    virtual const qint16 Version() const override;
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;

protected:
    virtual int SetGlobalParameters(CParameterPlugin *pPara) override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

private:
    CFrmFileTransfer* m_frmFileTransfer;
    CParameterFileTransfer m_Parameter;

protected:
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

    virtual int Initial() override;
    virtual int Clean() override;
};
