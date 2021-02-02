#ifndef FRMTERMINALAPPEARANCESETTINGS_H
#define FRMTERMINALAPPEARANCESETTINGS_H

#include <QWidget>
#include "ParameterTerminal.h"
#include "rabbitremotecontrolterminal_export.h"

namespace Ui {
class CFrmParameterTerminalAppearanceSettings;
}

class RABBITREMOTECONTROLTERMINAL_EXPORT CFrmParameterTerminalAppearanceSettings : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmParameterTerminalAppearanceSettings(CParameterTerminal* pPara, QWidget *parent = nullptr);
    virtual ~CFrmParameterTerminalAppearanceSettings() override;

    int AcceptSettings();

private slots:
    void on_fontComboBox_currentFontChanged(const QFont &f);
    void on_spFontSize_valueChanged(int size);

    void on_pbBrower_clicked();
    
private:
    Ui::CFrmParameterTerminalAppearanceSettings *ui;
    CParameterTerminal *m_pPara;

};

#endif // FRMTERMINALAPPEARANCESETTINGS_H
