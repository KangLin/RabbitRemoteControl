// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>

namespace Ui {
class CDlgSettingsTemplateQtEvent;
}

class CParameterTemplateQtEvent;
class CDlgSettingsTemplateQtEvent : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsTemplateQtEvent(CParameterTemplateQtEvent* pPara,
                                      QWidget *parent = nullptr);
    virtual ~CDlgSettingsTemplateQtEvent();

    // QDialog interface
public Q_SLOTS:
    virtual void accept() override;

private:
    Ui::CDlgSettingsTemplateQtEvent *ui;
    CParameterTemplateQtEvent* m_pPara;
};
