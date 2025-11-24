// Author: Kang Lin <kl222@126.com>

#pragma once

#include "Operate.h"
#include "FrmWebBrowser.h"
#include "ParameterWebBrowser.h"

class COperateWebBrowser : public COperate
{
    Q_OBJECT
public:
    COperateWebBrowser(CPlugin *plugin);
    virtual ~COperateWebBrowser();

public:
    [[nodiscard]] virtual const QString Name() override;
    [[nodiscard]] virtual const QIcon Icon() const override;
    [[nodiscard]] virtual const qint16 Version() const override;
    [[nodiscard]] virtual QWidget *GetViewer() override;
    [[nodiscard]] virtual QMenu* GetMenu(QWidget* parent = nullptr) override;
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
    virtual int Load(QSettings &set) override;
    virtual int Save(QSettings &set) override;

private:
    CFrmWebBrowser* m_pWeb;
    CParameterWebBrowser m_Parameter;
};
