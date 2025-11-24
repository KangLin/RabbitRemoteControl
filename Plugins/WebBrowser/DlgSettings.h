// Author: Kang Lin <kl222@126.com>
#pragma once
#include <QDialog>
#include <QStandardItemModel>
#include "ParameterWebBrowser.h"

namespace Ui {
class CDlgSettings;
}

class CDlgSettings : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgSettings(CParameterWebBrowser* para, QWidget *parent = nullptr);
    ~CDlgSettings();

public slots:
    virtual void accept() override;

private slots:
    void on_leHomeUrl_editingFinished();

    void on_pbDownloadFolder_clicked();

private:
    Ui::CDlgSettings *ui;
    CParameterWebBrowser* m_pPara;
    QStandardItemModel* m_pSearchModel;
};
