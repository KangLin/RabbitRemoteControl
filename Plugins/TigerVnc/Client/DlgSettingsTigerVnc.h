// Author: Kang Lin <kl222@126.com>

#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "ConnectTigerVnc.h"
#include "ParameterTigerVnc.h"
#include "ParameterProxyUI.h"

namespace Ui {
class CDlgSettingsTigerVnc;
}

class CDlgSettingsTigerVnc : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsTigerVnc(CParameterTigerVnc *pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettingsTigerVnc();
    
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
    void on_cbCompressAutoSelect_stateChanged(int arg1);
    void on_cbCompress_stateChanged(int arg1);
    void on_cbJPEG_stateChanged(int arg1);

    void on_gpIce_clicked(bool checked);
    
private:
    Ui::CDlgSettingsTigerVnc *ui;
    CParameterTigerVnc* m_pPara;
    CParameterProxyUI* m_pProxy;
};

#endif // DLGSETTINGS_H
