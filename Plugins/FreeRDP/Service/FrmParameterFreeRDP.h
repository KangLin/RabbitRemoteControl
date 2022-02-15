// Author: Kang Lin <kl222@126.com>

#ifndef FRMPARAMETERFREERDP_H
#define FRMPARAMETERFREERDP_H

#include <QWidget>
#include "ParameterServiceFreeRDP.h"

namespace Ui {
class CFrmParameterFreeRDP;
}

class CFrmParameterFreeRDP : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmParameterFreeRDP(CParameterServiceFreeRDP *para);
    ~CFrmParameterFreeRDP();
    
    /// ![slotSave]
public Q_SLOTS:
    //! Save parameters
    void slotSave();
    /// ![slotSave]

private:
    Ui::CFrmParameterFreeRDP *ui;
    
    CParameterServiceFreeRDP* m_Para;
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // FRMPARAMETERFREERDP_H
