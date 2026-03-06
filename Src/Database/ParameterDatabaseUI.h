// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterUI.h"
#include "ParameterDatabase.h"

namespace Ui {
class CParameterDatabaseUI;
}

class PLUGIN_EXPORT CParameterDatabaseUI : public CParameterUI
{
    Q_OBJECT

public:
    explicit CParameterDatabaseUI(QWidget *parent = nullptr);
    ~CParameterDatabaseUI();

    virtual int SetParameter(CParameter *pParameter) override;
    virtual bool CheckValidity(bool validity) override;
    virtual int Accept() override;

    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void slotTypeCurrentTextChanged(const QString &text);
    void on_pbBrowser_clicked();
    
    void on_rbSaveSettingsToDatabase_toggled(bool checked);
    
private:
    Ui::CParameterDatabaseUI *ui;
    CParameterDatabase* m_pParaDB;
    CParameterGlobal* m_pParaGlobal;
};
