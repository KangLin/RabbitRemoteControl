// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>
#include <QStandardItemModel>
#include "ParameterFtpServer.h"
#include "ParameterServerUI.h"
#include "ParameterFilterUI.h"

namespace Ui {
class CDlgSettings;
}

class CDlgSettings : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettings(CParameterFtpServer* para, QWidget *parent = nullptr);
    ~CDlgSettings();

public slots:
    virtual void accept() override;


private:
    Ui::CDlgSettings *ui;
    CParameterFtpServer* m_Para;
    CParameterServerUI* m_pServerUI;
    CParameterFilterUI* m_pWhitelist;
    CParameterFilterUI* m_pBlackList;
};
