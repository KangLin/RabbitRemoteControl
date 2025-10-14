// Author: Kang Lin <kl222@126.com>

#ifndef DLGINPUTPASSWORD_H
#define DLGINPUTPASSWORD_H

#include <QDialog>

namespace Ui {
class CDlgInputPassword;
}

class CDlgInputPassword : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgInputPassword(bool bShow = false,
                               bool bStore = true,
                               QWidget *parent = nullptr);
    virtual ~CDlgInputPassword();

    QString GetPassword();

private slots:
    void on_pbNo_clicked();
    void on_pbYes_clicked();
    void on_pbShow_clicked();
    
private:
    Ui::CDlgInputPassword *ui;
};

#endif // DLGINPUTPASSWORD_H
