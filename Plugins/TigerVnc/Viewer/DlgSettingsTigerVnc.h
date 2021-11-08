// Author: Kang Lin <kl222@126.com>

#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "ConnectTigerVnc.h"

namespace Ui {
class CDlgSettingsTigerVnc;
}

class CDlgSettingsTigerVnc : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsTigerVnc(CConnectTigerVnc::strPara *pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettingsTigerVnc();
    
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    
    void on_cbCompressAutoSelect_stateChanged(int arg1);

    void on_cbCompress_stateChanged(int arg1);
    
    void on_cbJPEG_stateChanged(int arg1);
    
    void on_pbShow_clicked();
        
    void on_gpIce_clicked(bool checked);
    
private:
    Ui::CDlgSettingsTigerVnc *ui;
    CConnectTigerVnc::strPara* m_pPara;
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // DLGSETTINGS_H
