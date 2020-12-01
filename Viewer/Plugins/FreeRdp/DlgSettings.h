#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>

namespace Ui {
class CDlgSettings;
}

class CDlgSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettings(QWidget *parent = nullptr);
    ~CDlgSettings();
    
private slots:
    void on_pbOk_clicked();
    
    void on_pbCancle_clicked();
    
private:
    Ui::CDlgSettings *ui;
};

#endif // DLGSETTINGS_H
