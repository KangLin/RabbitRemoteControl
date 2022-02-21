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
    
protected:
    virtual void showEvent(QShowEvent *event) override;
    
private:
    Ui::CFrmParameterServiceLibVNC *ui;
    CParameterServiceLibVNC* m_pParameters;
};

#endif // FRMPARAMETERSERVICELIBVNC_H
