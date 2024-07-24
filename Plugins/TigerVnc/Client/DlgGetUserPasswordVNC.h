#ifndef DLGGETUSERPASSWORD_H
#define DLGGETUSERPASSWORD_H

#include <QDialog>
#include "ConnecterVnc.h"

namespace Ui {
class CDlgGetPasswordVNC;
}

class CDlgGetPasswordVNC : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgGetPasswordVNC(QWidget *parent = nullptr);
    virtual ~CDlgGetPasswordVNC();
    CDlgGetPasswordVNC(const CDlgGetPasswordVNC& dlg);
    
    Q_INVOKABLE void SetContext(void* pContext);
    Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);
        
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgGetPasswordVNC *ui;
    CConnecterVnc* m_pConnecter;
    CParameterVnc* m_pParameter;
};
Q_DECLARE_METATYPE(CDlgGetPasswordVNC)

#endif // DLGGETUSERPASSWORD_H
