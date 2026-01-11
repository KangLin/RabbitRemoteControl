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
                                   QWidget *parent = nullptr);
    virtual ~CParameterDlgSettings();
    void SetViewers(const QList<QWidget*> &wViewer);
private slots:    
    void on_pbOk_clicked();
    void on_pbNo_clicked();

    void on_cbShowSystemTrayIcon_toggled(bool checked);
   
private:
    Ui::CParameterDlgSettings *ui;
    
    CParameterApp* m_pParameters;
};

#endif // DLGSETTINGS_H
