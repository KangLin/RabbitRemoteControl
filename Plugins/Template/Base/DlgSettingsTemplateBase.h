// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>

namespace Ui {
class CDlgSettingsTemplateBase;
}

class CParameterTemplateBase;
class CDlgSettingsTemplateBase : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettingsTemplateBase(CParameterTemplateBase* pPara,
                                      QWidget *parent = nullptr);
    virtual ~CDlgSettingsTemplateBase();

private:
    Ui::CDlgSettingsTemplateBase *ui;
    CParameterTemplateBase* m_pPara;
};
