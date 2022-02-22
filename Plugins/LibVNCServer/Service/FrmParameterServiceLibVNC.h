#ifndef FRMPARAMETERSERVICELIBVNC_H
#define FRMPARAMETERSERVICELIBVNC_H

#include <QWidget>
#include "ParameterServiceLibVNC.h"

namespace Ui {
class CFrmParameterServiceLibVNC;
}

class CFrmParameterServiceLibVNC : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmParameterServiceLibVNC(CParameterServiceLibVNC *para = nullptr);
    ~CFrmParameterServiceLibVNC();
    
    //! [Save parameters]
public Q_SLOTS:
    void slotSave();
    //! [Save parameters]

private:
    Ui::CFrmParameterServiceLibVNC *ui;
    CParameterServiceLibVNC* m_pParameters;
    
    void Init();
};

#endif // FRMPARAMETERSERVICELIBVNC_H
