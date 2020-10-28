#ifndef DLGPASSWORD_H
#define DLGPASSWORD_H

#include <QDialog>
#include "Connect.h"

namespace Ui {
class CDlgPassword;
}

class CDlgPassword : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgPassword(CConnect* pConnect, QWidget *parent = nullptr);
    ~CDlgPassword();
    
private slots:
    void on_pbOk_clicked();
    
    void on_pbCancle_clicked();
    
private:
    Ui::CDlgPassword *ui;
    CConnect* m_pConnect;
};

#endif // DLGPASSWORD_H
