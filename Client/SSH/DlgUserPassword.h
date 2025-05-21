#ifndef DLGUSERPASSWORD_H
#define DLGUSERPASSWORD_H

#include <QDialog>
#include "ParameterSSHTunnel.h"
#include "Connecter.h"

namespace Ui {
class CDlgUserPassword;
}

class CDlgUserPassword : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgUserPassword(QWidget *parent = nullptr);
    ~CDlgUserPassword();
    CDlgUserPassword(const CDlgUserPassword& other);
    
    Q_INVOKABLE void SetContext(void* pContext);
    Q_INVOKABLE void SetConnecter(CConnecter *pConnecter);

private:
    Ui::CDlgUserPassword *ui;
    CParameterSSHTunnel* m_pPara;
    CConnecter* m_pConnecter;
    
    // QDialog interface
public slots:
    virtual void accept() override;
};

Q_DECLARE_METATYPE(CDlgUserPassword)

#endif // DLGUSERPASSWORD_H
