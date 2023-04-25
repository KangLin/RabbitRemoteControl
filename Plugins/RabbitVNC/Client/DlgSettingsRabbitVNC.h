// Author: Kang Lin <kl222@126.com>

#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "ConnectRabbitVNC.h"
#include "ParameterRabbitVNC.h"

namespace Ui {
class CDlgSettingsRabbitVNC;
}

class CDlgSettingsRabbitVNC : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsRabbitVNC(CParameterRabbitVNC *pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettingsRabbitVNC();
    
private slots:
    void on_cbCompressAutoSelect_stateChanged(int arg1);

    void on_cbCompress_stateChanged(int arg1);
    
    void on_cbJPEG_stateChanged(int arg1);
    
    void on_pbShow_clicked();
        
    void on_gpIce_clicked(bool checked);
    
    void on_pbOk_clicked();
    
    void on_pbCancel_clicked();
    
    void on_leServer_editingFinished();
    
private:
    Ui::CDlgSettingsRabbitVNC *ui;
    CParameterRabbitVNC* m_pPara;
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // DLGSETTINGS_H
