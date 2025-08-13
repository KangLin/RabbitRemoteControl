// Author: Kang Lin <kl222@126.com>
#pragma once

#include <QDialog>
#include "ParameterTerminalUI.h"
#include "ParameterSerialPort.h"

namespace Ui {
class CDlgSettingsSerialPort;
}

class CDlgSettingsSerialPort : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsSerialPort(CParameterSerialPort* pPara, QWidget *parent = nullptr);
    ~CDlgSettingsSerialPort();

private:
    Ui::CDlgSettingsSerialPort *ui;
    CParameterSerialPort* m_pPara;
    CParameterTerminalUI* m_pFrmParaAppearnce;
    
    // QDialog interface
public slots:
    virtual void accept() override;
};
