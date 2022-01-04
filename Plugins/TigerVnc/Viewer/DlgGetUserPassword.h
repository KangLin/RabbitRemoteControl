#ifndef DLGGETUSERPASSWORD_H
#define DLGGETUSERPASSWORD_H

#include <QDialog>
#include "ConnecterTigerVnc.h"

namespace Ui {
class CDlgTigerVNCGetPassword;
}

class CDlgTigerVNCGetPassword : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgTigerVNCGetPassword(QWidget *parent = nullptr);
    virtual ~CDlgTigerVNCGetPassword();
    CDlgTigerVNCGetPassword(const CDlgTigerVNCGetPassword& dlg);
    
    Q_INVOKABLE void SetContext(void* pContext);
    Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);
        
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
    
private:
    Ui::CDlgTigerVNCGetPassword *ui;
    CConnecterTigerVnc* m_pConnecter;
    CParameterTigerVnc* m_pParameter;
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};
Q_DECLARE_METATYPE(CDlgTigerVNCGetPassword)
#endif // DLGGETUSERPASSWORD_H
