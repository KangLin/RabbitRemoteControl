// Author: Kang Lin <kl222@126.com>

#pragma once

#include "ParameterUI.h"
#include "ParameterTerminal.h"

namespace Ui {
class CParameterTerminalUI;
}

class PLUGIN_EXPORT CParameterTerminalUI : public CParameterUI
{
    Q_OBJECT

public:
    explicit CParameterTerminalUI(QWidget *parent = nullptr);
    virtual ~CParameterTerminalUI() override;

    virtual int SetParameter(CParameter *pParameter) override;
    virtual int Accept() override;

private slots:
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_spFontSize_valueChanged(int size);

    void on_pbBrowser_clicked();

private:
    Ui::CParameterTerminalUI *ui;
    CParameterTerminal *m_pPara;
};
