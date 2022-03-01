#ifndef FRMPARAMETERRABBITVNC_H
#define FRMPARAMETERRABBITVNC_H

#include <QWidget>
#include "ParameterServiceTigerVNC.h"

namespace Ui {
class CFrmParameterTigerVNC;
}

class CFrmParameterTigerVNC : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmParameterTigerVNC(CParameterServiceTigerVNC *para = nullptr);
    ~CFrmParameterTigerVNC();
    
    //! [Save parameters]
public Q_SLOTS:
    void slotSave();
    //! [Save parameters]
    
private slots:
    void on_pbShow_clicked();
    
private:
    void Init();
    
    Ui::CFrmParameterTigerVNC *ui;
    CParameterServiceTigerVNC* m_pParameters;
};

#endif // FRMPARAMETERRABBITVNC_H
