// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once

#include <QAbstractButton>
#include <QDialog>
#include "ParameterFileTransfer.h"

namespace Ui {
class CDlgFileTransfer;
}

class CDlgFileTransfer : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgFileTransfer(CParameterFileTransfer* pPara, QWidget *parent = nullptr);
    virtual ~CDlgFileTransfer();

private:
    Ui::CDlgFileTransfer *ui;
    CParameterFileTransfer* m_pPara;

public slots:
    virtual void accept() override;
private slots:
    void on_cbProtocol_currentIndexChanged(int index);
};
