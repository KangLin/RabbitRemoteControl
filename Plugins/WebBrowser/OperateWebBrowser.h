// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Operate.h"
#include "FrmWebBrowser.h"

class COperateWebBrowser : public COperate
{
    Q_OBJECT
public:
    COperateWebBrowser(CPlugin *plugin);
    virtual ~COperateWebBrowser();

public:
    virtual const QString Name() override;
    virtual const QIcon Icon() const override;
    virtual const qint16 Version() const override;
    virtual QWidget *GetViewer() override;
    virtual int Start() override;
    virtual int Stop() override;

protected:
    virtual int SetGlobalParameters(CParameterPlugin *pPara) override;

private:
    virtual QDialog *OnOpenDialogSettings(QWidget *parent) override;

    // COperate interface
protected:
    virtual int Initial() override;
    virtual int Clean() override;

private:
    CFrmWebBrowser* m_pWeb;
};
