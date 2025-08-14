// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterUI.h"
#include "ParameterRecord.h"

namespace Ui {
class CParameterRecordUI;
}

class PLUGIN_EXPORT CParameterRecordUI : public CParameterUI
{
    Q_OBJECT

public:
    explicit CParameterRecordUI(QWidget *parent = nullptr);
    ~CParameterRecordUI();

public Q_SLOTS:
    virtual int SetParameter(CParameter *pParameter) override;
    virtual int Accept() override;

    void on_pbFolder_clicked();
    void on_cmbEncodingMode_currentIndexChanged(int index);

private:
    Ui::CParameterRecordUI *ui;
    CParameterRecord* m_pParameters;
};
