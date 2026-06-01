// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>

namespace Ui {
class CDlgSettingsTemplateDesktop;
}

class CParameterTemplateDesktop;
class CDlgSettingsTemplateDesktop : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsTemplateDesktop(CParameterTemplateDesktop* pPara,
                                      QWidget *parent = nullptr);
    virtual ~CDlgSettingsTemplateDesktop();

    // QDialog interface
public Q_SLOTS:
    virtual void accept() override;

private:
    Ui::CDlgSettingsTemplateDesktop *ui;
    CParameterTemplateDesktop* m_pPara;
};
