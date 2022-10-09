// Author: Kang Lin <kl222@126.com>

#ifndef FRMPARAMETERFREERDP_H
#define FRMPARAMETERFREERDP_H

#include <QLoggingCategory>
#include <QWidget>
#include "ParameterServiceFreeRDP.h"

namespace Ui {
class CFrmParameterFreeRDP;
}

class CFrmParameterFreeRDP : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmParameterFreeRDP(CParameterServiceFreeRDP *para, QWidget* parent = nullptr);
    ~CFrmParameterFreeRDP();
    
    //! [Accept parameters]
public Q_SLOTS:
    /*!
     * \brief Accept parameters
     */
    void slotAccept();
    //! [Accept parameters]

private:
    Ui::CFrmParameterFreeRDP *ui;
    QLoggingCategory m_Log;
    CParameterServiceFreeRDP* m_Para;
    
    void Init();
};

#endif // FRMPARAMETERFREERDP_H
