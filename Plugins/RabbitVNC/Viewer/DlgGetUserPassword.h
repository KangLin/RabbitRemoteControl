#ifndef DLGGETUSERPASSWORD_H
#define DLGGETUSERPASSWORD_H

#include <QDialog>
#include "ConnecterRabbitVNC.h"

namespace Ui {
class CDlgGetPasswordRabbitVNC;
}

class CDlgGetPasswordRabbitVNC : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgGetPasswordRabbitVNC(QWidget *parent = nullptr);
    virtual ~CDlgGetPasswordRabbitVNC();
    CDlgGetPasswordRabbitVNC(const CDlgGetPasswordRabbitVNC& dlg);
    
    Q_INVOKABLE void SetContext(void* pContext);
    Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);
        
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgGetPasswordRabbitVNC *ui;
    CConnecterRabbitVNC* m_pConnecter;
    CParameterRabbitVNC* m_pParameter;
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};
Q_DECLARE_METATYPE(CDlgGetPasswordRabbitVNC)
#endif // DLGGETUSERPASSWORD_H
