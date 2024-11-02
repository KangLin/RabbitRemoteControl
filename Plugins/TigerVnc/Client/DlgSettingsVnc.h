// Author: Kang Lin <kl222@126.com>

#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "ParameterVnc.h"
#include "ParameterProxyUI.h"
#include "ParameterRecordUI.h"

namespace Ui {
class CDlgSettingsVnc;
}

class CDlgSettingsVnc : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsVnc(CParameterVnc *pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettingsVnc();
    
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
    void on_cbCompressAutoSelect_stateChanged(int arg1);
    void on_cbCompress_stateChanged(int arg1);
    void on_cbJPEG_stateChanged(int arg1);

    void on_gpIce_clicked(bool checked);
    
private:
    Ui::CDlgSettingsVnc *ui;
    CParameterVnc* m_pPara;
    CParameterProxyUI* m_pProxyUI;
    CParameterRecordUI* m_pRecordUI;
};

#endif // DLGSETTINGS_H
