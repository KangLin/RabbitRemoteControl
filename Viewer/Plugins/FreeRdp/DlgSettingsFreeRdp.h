//! @author: Kang Lin(kl222@126.com)

#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "freerdp/freerdp.h"

namespace Ui {
class CDlgSettingsFreeRdp;
}

class CDlgSettingsFreeRdp : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsFreeRdp(rdpSettings* pSettings, QWidget *parent = nullptr);
    ~CDlgSettingsFreeRdp();
    
private slots:
    void on_pbOk_clicked();
    
    void on_pbCancle_clicked();
    
private:
    Ui::CDlgSettingsFreeRdp *ui;
    rdpSettings* m_pSettings;
};

#endif // DLGSETTINGS_H
