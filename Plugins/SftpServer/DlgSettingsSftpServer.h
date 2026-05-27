// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>
#include "ParameterServerUI.h"
#include "ParameterFilterUI.h"

namespace Ui {
class CDlgSettingsSftpServer;
}

class CParameterSftpServer;
class CDlgSettingsSftpServer : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsSftpServer(CParameterSftpServer* pPara,
                                      QWidget *parent = nullptr);
    virtual ~CDlgSettingsSftpServer();

    // QDialog interface
public slots:
    virtual void accept() override;

private:
    Ui::CDlgSettingsSftpServer *ui;
    CParameterSftpServer* m_pPara;
    CParameterServerUI* m_pServerUI;
    CParameterFilterUI* m_pWhitelist;
    CParameterFilterUI* m_pBlacklist;
};
