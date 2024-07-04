// Author: Kang Lin <kl222@126.com>

#ifndef DLGSETFREERDP_H
#define DLGSETFREERDP_H

#include <QDialog>
#include "freerdp/freerdp.h"
#include "ConnecterFreeRDP.h"
#include <QFileSystemModel>
#include <QItemSelection>

namespace Ui {
class CDlgSetFreeRDP;
}

class CDlgSetFreeRDP : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSetFreeRDP(CParameterFreeRDP* pSettings, QWidget *parent = nullptr);
    ~CDlgSetFreeRDP();
    
private slots:
    void on_pbOk_clicked();
    void on_pbCancel_clicked();
    void on_rbLocalScreen_clicked(bool checked);
    void on_leServer_editingFinished();

    void on_pbShow_clicked();

    void on_rbAudioDisable_toggled(bool checked);
    void on_rbAudioLocal_toggled(bool checked);
    void on_rbAudioRemote_toggled(bool checked);
    
    void on_cbSavePassword_stateChanged(int arg1);
    
    void on_pbSizeEdit_clicked();
    
    void on_pbDriveClearAll_clicked();
    void slotSelectionChanged(const QItemSelection &selected,
                              const QItemSelection &deselected);
    
    void on_cbAllMonitor_stateChanged(int arg1);
    
private:
    QRect GetScreenGeometry();
    int UpdateDesktopSize();
    int InsertDesktopSize(QString szSize);
    int InsertDesktopSize(int width, int height);
    bool HasAudioOutput();
    bool HasAudioInput();
    int ShowDriveSelected(int counts);

private:
    Ui::CDlgSetFreeRDP *ui;
    CParameterFreeRDP* m_pSettings;
    QFileSystemModel* m_pFileModel;

};

#endif // DLGSETFREERDP_H
