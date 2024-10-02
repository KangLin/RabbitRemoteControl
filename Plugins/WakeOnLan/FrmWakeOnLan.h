#ifndef FRMWAKEONLAN_H
#define FRMWAKEONLAN_H

#include <QWidget>
#include "ParameterWakeOnLan.h"
#include "ParameterUI.h"

namespace Ui {
class CFrmWakeOnLan;
}

class CFrmWakeOnLan : public CParameterUI
{
    Q_OBJECT

public:
    explicit CFrmWakeOnLan(QWidget *parent = nullptr);
    ~CFrmWakeOnLan();

Q_SIGNALS:
    void sigOk();
    void sigCancel();

private slots:
    void on_pbOK_clicked();

private:
    Ui::CFrmWakeOnLan *ui;
    CParameterWakeOnLan* m_pParameter;

    // CParameterUI interface
public:
    virtual int SetParameter(CParameter *pParameter) override;
    virtual int Accept() override;
};

#endif // FRMWAKEONLAN_H
