// Author: Kang Lin <kl222@126.com>

#pragma once

#pragma once

#include "ParameterUI.h"
#include "ParameterSSH.h"

namespace Ui {
class CParameterSSHTunnelUI;
}

class CParameterSSHTunnelUI : public CParameterUI
{
    Q_OBJECT

public:
    explicit CParameterSSHTunnelUI(QWidget *parent = nullptr);
    ~CParameterSSHTunnelUI();

private:
    Ui::CParameterSSHTunnelUI *ui;
    CParameterSSHTunnel* m_pParameters;

    // CParameterUI interface
public:
    virtual int SetParameter(CParameter *pParameter) override;
    virtual int Accept() override;
    bool CheckValidity(bool validity = false) override;
private slots:
    void on_pbBrowser_clicked();
};
