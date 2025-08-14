// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterUI.h"
#include "ParameterPlugin.h"

namespace Ui {
class CParameterPluginUI;
}

class CParameterPluginUI : public CParameterUI
{
    Q_OBJECT

public:
    explicit CParameterPluginUI(QWidget *parent = nullptr);
    virtual ~CParameterPluginUI();

public Q_SLOTS:
    void on_cbEnableViewPassword_clicked(bool checked);
    void on_pbEncryptKey_clicked();

private:
    Ui::CParameterPluginUI *ui;
    CParameterPlugin* m_pPara;

    // CParameterUI interface
public:
    virtual int SetParameter(CParameter *pParameter) override;
    virtual int Accept() override;
};
