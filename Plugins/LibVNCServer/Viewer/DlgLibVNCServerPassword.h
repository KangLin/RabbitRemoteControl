#ifndef DLGLIBVNCSERVERPASSWORD_H
#define DLGLIBVNCSERVERPASSWORD_H

#include <QDialog>
#include "ConnecterLibVNCServer.h"

namespace Ui {
class CDlgLibVNCServerPassword;
}

class CDlgLibVNCServerPassword : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgLibVNCServerPassword(QWidget *parent = nullptr);
    CDlgLibVNCServerPassword(const CDlgLibVNCServerPassword& d);
    ~CDlgLibVNCServerPassword();
    
    Q_INVOKABLE void SetContext(void* pContext);
    Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);
    
private:
    Ui::CDlgLibVNCServerPassword *ui;
    CConnecterLibVNCServer* m_pConnecter;
    CParameterLibVNCServer* m_pParameter;
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
private slots:
    void on_pbOK_clicked();
    void on_pbCancel_clicked();
};

Q_DECLARE_METATYPE(CDlgLibVNCServerPassword)
#endif // DLGLIBVNCSERVERPASSWORD_H
