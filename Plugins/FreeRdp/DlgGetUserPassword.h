#ifndef DLGGETUSERPASSWORD_H
#define DLGGETUSERPASSWORD_H

#include <QDialog>

#include "ConnecterFreeRdp.h"

namespace Ui {
class CDlgGetUserPassword;
}

class CDlgGetUserPassword : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgGetUserPassword(QWidget *parent = nullptr);
    virtual ~CDlgGetUserPassword();
    CDlgGetUserPassword(const CDlgGetUserPassword& dlg);
    
    Q_INVOKABLE void SetContext(void* pContext);
    Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);

private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgGetUserPassword *ui;
    
    CConnecterFreeRdp* m_pConnecter;
    CParameterFreeRdp* m_pParameter;

    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};

Q_DECLARE_METATYPE(CDlgGetUserPassword)

#endif // DLGGETUSERPASSWORD_H
