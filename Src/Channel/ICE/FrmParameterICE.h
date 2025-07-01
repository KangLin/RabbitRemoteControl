//! @author Kang Lin <kl222@126.com>

#ifndef CFRMPARAMETERICE_H
#define CFRMPARAMETERICE_H

#include <QWidget>
#include "ParameterICE.h"

namespace Ui {
class CFrmParameterICE;
}

/*!
 * \~chinese
 * \brief 设置参数对话框
 *
 * \~english
 * \brief Set parameter dialog
 * 
 * \~
 * \ingroup LIBAPI_ICE
 */
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
    void slotConnected();
    void slotDisconnected();
    void slotError(int nError, const QString& szError);
    
    void on_cbEnableICE_stateChanged(int arg1);
    void on_pbConnect_clicked(bool checked);
    void on_leSignalName_editingFinished();
    
private:
    void Init();
    void EnableCompone(bool bEnable);
    void SetConnectButton(bool bConnected);
    
    Ui::CFrmParameterICE *ui;
    CParameterICE* m_pParameters;
};

#endif // CFRMPARAMETERICE_H
