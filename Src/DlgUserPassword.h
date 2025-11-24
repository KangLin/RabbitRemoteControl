// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>
#include "ParameterNet.h"

namespace Ui {
class CDlgUserPassword;
}

class PLUGIN_EXPORT CDlgUserPassword : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgUserPassword(QWidget *parent = nullptr);
    ~CDlgUserPassword();
    CDlgUserPassword(const CDlgUserPassword& other);
    
    Q_INVOKABLE void SetContext(void* pContext);
    int SetUser(const QString &szPrompt, CParameterUser* pUser);

private:
    Ui::CDlgUserPassword *ui;
    CParameterUser* m_pUser;
    
    // QDialog interface
public slots:
    virtual void accept() override;
};

Q_DECLARE_METATYPE(CDlgUserPassword)
