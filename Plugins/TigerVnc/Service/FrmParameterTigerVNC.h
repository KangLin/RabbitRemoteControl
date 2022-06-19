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
    explicit CFrmParameterTigerVNC(CParameterServiceTigerVNC *para, QWidget *parent = nullptr);
    ~CFrmParameterTigerVNC();
    
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
    Ui::CFrmParameterTigerVNC *ui;
    CParameterServiceTigerVNC* m_pParameters;
};

#endif // FRMPARAMETERRABBITVNC_H
