#ifndef DLGSETTINGS_H
#define DLGSETTINGS_H

#include <QDialog>
#include "ConnectTigerVnc.h"

namespace Ui {
class CDlgSettings;
}

class CDlgSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettings(CConnectTigerVnc::strPara *pPara, QWidget *parent = nullptr);
    virtual ~CDlgSettings();
    
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    
    void on_cbCompressAutoSelect_stateChanged(int arg1);

    void on_cbCompress_stateChanged(int arg1);
    
    void on_cbJPEG_stateChanged(int arg1);
    
private:
    Ui::CDlgSettings *ui;
    CConnectTigerVnc::strPara* m_pPara;
};

#endif // DLGSETTINGS_H
