#ifndef FRMPARAMETERVIEWER_H
#define FRMPARAMETERVIEWER_H

#include "ParameterUI.h"
#include "ParameterClient.h"

namespace Ui {
class CFrmParameterClient;
}

class CFrmParameterClient : public CParameterUI
{
    Q_OBJECT

public:
    explicit CFrmParameterClient(QWidget *parent = nullptr);
    virtual ~CFrmParameterClient();

public Q_SLOTS:
    void on_cbEnableViewPassword_clicked(bool checked);
    void on_pbEncryptKey_clicked();

private:
    Ui::CFrmParameterClient *ui;
    CParameterClient* m_pPara;

    // CParameterUI interface
public:
    virtual int SetParameter(CParameter *pParameter) override;
    virtual int Accept() override;
};

#endif // FRMPARAMETERVIEWER_H
