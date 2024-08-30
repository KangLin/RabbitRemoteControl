#ifndef DLGUSERPASSWORD_H
#define DLGUSERPASSWORD_H

#include <QDialog>
#include "ParameterChannelSSH.h"

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

private:
    Ui::CDlgUserPassword *ui;
    CParameterChannelSSH* m_pPara;
    
    // QDialog interface
public slots:
    virtual void accept() override;
};

Q_DECLARE_METATYPE(CDlgUserPassword)

#endif // DLGUSERPASSWORD_H
