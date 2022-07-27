#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "ParameterApp.h"

namespace Ui {
class CParameterDlgSettings;
}

class CParameterDlgSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit CParameterDlgSettings(CParameterApp* pPara,
                                   QList<QWidget*> wViewer,
                                   QWidget *parent = nullptr);
    virtual ~CParameterDlgSettings();

private slots:    
    void on_pbOk_clicked();
    void on_pbNo_clicked();

    void on_cbEnableViewPassword_clicked(bool checked);
    void on_pbEncryptKey_clicked();
    void on_pbScreenShotBrower_clicked();
    void on_cbShowSystemTrayIcon_toggled(bool checked);
    
private:
    Ui::CParameterDlgSettings *ui;
    
    CParameterApp* m_pParameters;
};

#endif // DLGSETTINGS_H
