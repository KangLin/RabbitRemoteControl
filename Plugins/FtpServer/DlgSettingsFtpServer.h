// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>
#include "ParameterServerUI.h"

namespace Ui {
class CDlgSettingsFtpServer;
}

class CParameterFtpServer;
class CDlgSettingsFtpServer : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsFtpServer(CParameterFtpServer* pPara,
                                      QWidget *parent = nullptr);
    virtual ~CDlgSettingsFtpServer();

    // QDialog interface
public Q_SLOTS:
    virtual void accept() override;
    void on_pbRoot_clicked();
private:
    Ui::CDlgSettingsFtpServer *ui;
    CParameterFtpServer* m_pPara;
    CParameterServerUI* m_pServerUI;
};
