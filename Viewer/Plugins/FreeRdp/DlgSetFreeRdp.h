#ifndef DLGSETFREERDP_H
#define DLGSETFREERDP_H

#include <QDialog>
#include "freerdp/freerdp.h"

namespace Ui {
class CDlgSetFreeRdp;
}

class CDlgSetFreeRdp : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSetFreeRdp(rdpSettings* pSettings, QWidget *parent = nullptr);
    ~CDlgSetFreeRdp();
    
private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
    void on_rbFullScreen_clicked(bool checked);
    
private:
    int InsertDesktopSize(int width, int height);
    
private:
    Ui::CDlgSetFreeRdp *ui;
    rdpSettings* m_pSettings;
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // DLGSETFREERDP_H
