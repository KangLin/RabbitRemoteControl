// Author: Kang Lin <kl222@126.com>

#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "ConnecterLibVNCServer.h"

namespace Ui {
class CDlgSettingsLibVnc;
}

class CDlgSettingsLibVnc : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsLibVnc(CConnecterLibVNCServer *pConnecter, QWidget *parent = nullptr);
    virtual ~CDlgSettingsLibVnc();
    
private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();

    void on_cbCompress_stateChanged(int arg1);
    void on_cbJPEG_stateChanged(int arg1);
    
    void on_pbShow_clicked();
    
    void on_leHost_editingFinished();
    
    void on_cbSavePassword_stateChanged(int arg1);
    
private:
    Ui::CDlgSettingsLibVnc *ui;
    CParameterLibVNCServer* m_pPara;
};

#endif // DLGSETTINGS_H
