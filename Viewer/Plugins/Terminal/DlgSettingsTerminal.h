#ifndef DLGSETTINGSTERMINAL_H
#define DLGSETTINGSTERMINAL_H

#include <QDialog>

namespace Ui {
class CDlgSettingsTerminal;
}

class CDlgSettingsTerminal : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettingsTerminal(QWidget *parent = nullptr);
    ~CDlgSettingsTerminal();
    
private slots:
    void on_pushButton_clicked();
    
    void on_pushButton_2_clicked();
    
private:
    Ui::CDlgSettingsTerminal *ui;
};

#endif // DLGSETTINGSTERMINAL_H
