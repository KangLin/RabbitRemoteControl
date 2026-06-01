// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QWidget>

namespace Ui {
class CFrmViewerTemplateBase;
}

class CFrmViewerTemplateBase : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmViewerTemplateBase(QWidget *parent = nullptr);
    ~CFrmViewerTemplateBase();

private:
    Ui::CFrmViewerTemplateBase *ui;
};
