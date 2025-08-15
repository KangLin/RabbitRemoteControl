// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>
#include "ParameterTerminalUI.h"
#include "ParameterRawStream.h"

namespace Ui {
class CDlgSettingsRawStream;
}

class CDlgSettingsRawStream : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsRawStream(CParameterRawStream* pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettingsRawStream();

private:
    Ui::CDlgSettingsRawStream *ui;
    CParameterTerminalUI* m_pFrmParaAppearnce;
    CParameterRawStream* m_pPara;
    
    // QDialog interface
public slots:
    virtual void accept() override;
private slots:
    void on_cbType_currentIndexChanged(int index);
};
