// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>
#include "ParameterServerUI.h"

namespace Ui {
class CDlgSettingsFreeRDPServer;
}

class CParameterFreeRDPServer;
class CDlgSettingsFreeRDPServer : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsFreeRDPServer(CParameterFreeRDPServer* pPara,
                                      QWidget *parent = nullptr);
    virtual ~CDlgSettingsFreeRDPServer();

    // QDialog interface
public Q_SLOTS:
    virtual void accept() override;

private:
    Ui::CDlgSettingsFreeRDPServer *ui;
    CParameterFreeRDPServer* m_pPara;
    CParameterServerUI* m_pServerUI;
};
