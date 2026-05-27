// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QStandardItemModel>
#include "ParameterUI.h"
#include "ParameterFilter.h"

namespace Ui {
class CParameterFilterUI;
}

class PLUGIN_EXPORT CParameterFilterUI : public CParameterUI
{
    Q_OBJECT

public:
    explicit CParameterFilterUI(QWidget *parent = nullptr);
    virtual ~CParameterFilterUI();

    virtual int SetParameter(CParameter *pParameter) override;
    virtual int Accept() override;

private Q_SLOTS:
    void slotContextMenuRequested(const QPoint& pos);
    void on_pbAdd_clicked();
    void on_pbDelete_clicked();

private:
    Ui::CParameterFilterUI *ui;
    QStandardItemModel m_Model;
    QString m_szFilteListPrompt;
    CParameterFilter* m_pPara;
};
