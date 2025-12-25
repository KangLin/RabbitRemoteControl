// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>
#include <QStandardItemModel>
#include "ParameterFtpServer.h"

namespace Ui {
class CDlgSettings;
}

class CDlgSettings : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettings(QSharedPointer<CParameterFtpServer> para, QWidget *parent = nullptr);
    ~CDlgSettings();

public slots:
    virtual void accept() override;
private slots:
    void on_pbRoot_clicked();
    
    void on_cbAnonmousLogin_checkStateChanged(const Qt::CheckState &arg1);
    
    void on_cbListenAll_checkStateChanged(const Qt::CheckState &arg1);
    
private:
    Ui::CDlgSettings *ui;
    QSharedPointer<CParameterFtpServer> m_Para;
    QStandardItemModel m_ModelNetWorkInterface;
};
