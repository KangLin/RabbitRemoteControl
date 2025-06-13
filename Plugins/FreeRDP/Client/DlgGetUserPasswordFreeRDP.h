#ifndef DLGGETUSERPASSWORD_H
#define DLGGETUSERPASSWORD_H

#include <QDialog>

#include "OperateFreeRDP.h"

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

private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgGetUserPasswordFreeRDP *ui;

    CParameterFreeRDP* m_pParameter;

};

Q_DECLARE_METATYPE(CDlgGetUserPasswordFreeRDP)

#endif // DLGGETUSERPASSWORD_H
