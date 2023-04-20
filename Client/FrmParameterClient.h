#ifndef FRMPARAMETERVIEWER_H
#define FRMPARAMETERVIEWER_H

#include <QWidget>
#include "ParameterClient.h"

namespace Ui {
class CFrmParameterClient;
}

class CFrmParameterClient : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmParameterClient(CParameterClient* pParameter,
                                 QWidget *parent = nullptr);
    virtual ~CFrmParameterClient();

    // [Accept parameters]
public Q_SLOTS:
    /*!
     * \brief Accept parameters
     */
    void slotAccept();
    // [Accept parameters]

    void on_cbEnableViewPassword_clicked(bool checked);
    void on_pbEncryptKey_clicked();

private:
    Ui::CFrmParameterClient *ui;
    CParameterClient* m_pPara;
};

#endif // FRMPARAMETERVIEWER_H
