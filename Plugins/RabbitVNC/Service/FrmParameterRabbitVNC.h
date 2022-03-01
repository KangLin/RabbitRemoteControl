#ifndef FRMPARAMETERRABBITVNC_H
#define FRMPARAMETERRABBITVNC_H

#include <QWidget>
#include "ParameterServiceRabbitVNC.h"

namespace Ui {
class CFrmParameterRabbitVNC;
}

class CFrmParameterRabbitVNC : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmParameterRabbitVNC(CParameterServiceRabbitVNC *para = nullptr);
    ~CFrmParameterRabbitVNC();
    
    //! [Save parameters]
public Q_SLOTS:
    void slotSave();
    //! [Save parameters]
        
private slots:
    void on_pbShow_clicked();
    
private:
    void Init();
    
    Ui::CFrmParameterRabbitVNC *ui;
    CParameterServiceRabbitVNC* m_pParameters;
};

#endif // FRMPARAMETERRABBITVNC_H
