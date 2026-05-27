// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QStandardItemModel>
#include "ParameterUI.h"
#include "ParameterServer.h"

namespace Ui {
class CParameterServerUI;
}

class PLUGIN_EXPORT CParameterServerUI : public CParameterUI
{
    Q_OBJECT

public:
    explicit CParameterServerUI(QWidget *parent = nullptr);
    ~CParameterServerUI();

    virtual int SetParameter(CParameter *pParameter) override;
    virtual bool CheckValidity(bool validity) override;
    virtual int Accept() override;

private slots:
    void on_pbRoot_clicked();
    void on_cbListenAll_checkStateChanged(const Qt::CheckState &arg1);
    void on_pbAddWhitelist_clicked();
    void on_pbDeleteWhitelist_clicked();
    void on_pbAddBlacklist_clicked();
    void on_pbDeleteBlacklist_clicked();
    void slotWhiteListContextMenuRequested(const QPoint& pos);
    void slotBlackListContextMenuRequested(const QPoint& pos);

private:
    Ui::CParameterServerUI *ui;
    CParameterServer* m_pPara;
    QStandardItemModel m_ModelNetWorkInterface;
    QStandardItemModel m_ModelBlack;
    QStandardItemModel m_ModelWhite;
    QString m_szFilteListPrompt;
};
