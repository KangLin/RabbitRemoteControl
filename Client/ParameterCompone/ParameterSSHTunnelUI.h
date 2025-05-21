#ifndef PARAMETERSSHTUNNELUI_H
#define PARAMETERSSHTUNNELUI_H

#pragma once

#include "ParameterUI.h"
#include "ParameterSSHTunnel.h"

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
    bool CheckValidity(bool validity = false);
};

#endif // PARAMETERSSHTUNNELUI_H
