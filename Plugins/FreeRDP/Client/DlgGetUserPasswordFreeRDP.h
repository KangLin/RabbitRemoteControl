#ifndef DLGGETUSERPASSWORD_H
#define DLGGETUSERPASSWORD_H

#include <QDialog>

#include "ConnecterFreeRDP.h"

namespace Ui {
class CDlgGetUserPasswordFreeRDP;
}

class CDlgGetUserPasswordFreeRDP : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgGetUserPasswordFreeRDP(QWidget *parent = nullptr);
    virtual ~CDlgGetUserPasswordFreeRDP();
    CDlgGetUserPasswordFreeRDP(const CDlgGetUserPasswordFreeRDP& dlg);
    
    Q_INVOKABLE void SetContext(void* pContext);
    Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);

private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgGetUserPasswordFreeRDP *ui;

    CConnecterFreeRDP* m_pConnecter;
    CParameterFreeRDP* m_pParameter;

    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};

Q_DECLARE_METATYPE(CDlgGetUserPasswordFreeRDP)

#endif // DLGGETUSERPASSWORD_H
