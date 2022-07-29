#ifndef DLGGETUSERPASSWORD_H
#define DLGGETUSERPASSWORD_H

#include <QDialog>
#include "ConnecterTigerVnc.h"

namespace Ui {
class CDlgGetPasswordTigerVNC;
}

class CDlgGetPasswordTigerVNC : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgGetPasswordTigerVNC(QWidget *parent = nullptr);
    virtual ~CDlgGetPasswordTigerVNC();
    CDlgGetPasswordTigerVNC(const CDlgGetPasswordTigerVNC& dlg);
    
    Q_INVOKABLE void SetContext(void* pContext);
    Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);
        
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgGetPasswordTigerVNC *ui;
    CConnecterTigerVnc* m_pConnecter;
    CParameterTigerVnc* m_pParameter;
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};
Q_DECLARE_METATYPE(CDlgGetPasswordTigerVNC)
#endif // DLGGETUSERPASSWORD_H
