#ifndef CFRMPARAMETERICE_H
#define CFRMPARAMETERICE_H

#include <QWidget>
#include "ParameterICE.h"

namespace Ui {
class CFrmParameterICE;
}

class CFrmParameterICE : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmParameterICE(CParameterICE *para = nullptr,
                              QWidget* parent = nullptr);
    virtual ~CFrmParameterICE();

    //! [Accept parameters]
public Q_SLOTS:
    /*!
     * \brief Accept parameter
     */
    void slotAccept();
    //! [Accept parameters]

private slots:
    void on_cbEnableICE_stateChanged(int arg1);
    void on_pbConnect_clicked(bool checked);
    
private:
    void EnableCompone(bool bEnable);
    
    Ui::CFrmParameterICE *ui;
    CParameterICE* m_pParameters;
    
    // QWidget interface
protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // CFRMPARAMETERICE_H
