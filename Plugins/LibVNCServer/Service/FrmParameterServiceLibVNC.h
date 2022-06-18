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
    explicit CFrmParameterServiceLibVNC(CParameterServiceLibVNC *para, QWidget* parent = nullptr);
    ~CFrmParameterServiceLibVNC();
    
    //! [Accept parameters]
public Q_SLOTS:
    void slotAccept();
    //! [Accept parameters]

private:
    Ui::CFrmParameterServiceLibVNC *ui;
    CParameterServiceLibVNC* m_pParameters;
    
    void Init();
};

#endif // FRMPARAMETERSERVICELIBVNC_H
