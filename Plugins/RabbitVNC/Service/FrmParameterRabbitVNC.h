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
    explicit CFrmParameterRabbitVNC(CParameterServiceRabbitVNC *para, QWidget* parent = nullptr);
    ~CFrmParameterRabbitVNC();
    
    //! [Accept parameters]
public Q_SLOTS:
    /*!
     * \brief Accept parameter
     */
    void slotAccept();
    //! [Accept parameters]
        
private slots:
    void on_pbShow_clicked();
    
private:    
    Ui::CFrmParameterRabbitVNC *ui;
    CParameterServiceRabbitVNC* m_pParameters;
};

#endif // FRMPARAMETERRABBITVNC_H
