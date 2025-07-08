// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>
#pragma once

#include <QWidget>

namespace Ui {
class CFrmFileTransfer;
}

class CFrmFileTransfer : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmFileTransfer(QWidget *parent = nullptr);
    ~CFrmFileTransfer();

private:
    Ui::CFrmFileTransfer *ui;
};
