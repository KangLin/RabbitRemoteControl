#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "ParameterApp.h"

namespace Ui {
class CDlgSettings;
}

class CDlgSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettings(CParameterApp* pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettings();
    
private slots:    
    void on_pbOk_clicked();
    void on_pbNo_clicked();
    
private:
    Ui::CDlgSettings *ui;
    
    CParameterApp* m_pParameters;
};

#endif // DLGSETTINGS_H
