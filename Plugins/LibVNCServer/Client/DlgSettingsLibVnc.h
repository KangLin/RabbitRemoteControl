// Author: Kang Lin <kl222@126.com>

#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "ConnecterLibVNCServer.h"
#include "ParameterProxyUI.h"

namespace Ui {
class CDlgSettingsLibVnc;
}

class CDlgSettingsLibVnc : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsLibVnc(CParameterLibVNCServer *pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettingsLibVnc();
    
private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();

    void on_cbCompress_stateChanged(int arg1);
    void on_cbJPEG_stateChanged(int arg1);
    
private:
    Ui::CDlgSettingsLibVnc *ui;
    CParameterLibVNCServer* m_pPara;
    CParameterProxyUI* m_uiProxy;
};

#endif // DLGSETTINGS_H
