// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QWidget>

namespace Ui {
class CFrmViewerTemplateQtEvent;
}

class CFrmViewerTemplateQtEvent : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmViewerTemplateQtEvent(QWidget *parent = nullptr);
    virtual ~CFrmViewerTemplateQtEvent();

private:
    Ui::CFrmViewerTemplateQtEvent *ui;
};
