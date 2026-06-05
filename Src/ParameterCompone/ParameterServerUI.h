// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QStandardItemModel>
#include "ParameterUI.h"
#include "ParameterFilterUI.h"
#include "ParameterServer.h"

namespace Ui {
class CParameterServerUI;
}

/*!
 * \brief The server base parameters UI
 * \ingroup gOperateServer
 */
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
    void on_cbListenAll_checkStateChanged(const Qt::CheckState &arg1);

private:
    Ui::CParameterServerUI *ui;
    CParameterServer* m_pPara;
    QStandardItemModel m_ModelNetWorkInterface;
    CParameterFilterUI* m_pWhitelist;
    CParameterFilterUI* m_pBlacklist;
};
