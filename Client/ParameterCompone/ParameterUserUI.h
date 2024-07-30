#ifndef PARAMETERUSERUI_H
#define PARAMETERUSERUI_H

#include <QWidget>
#include "ParameterUser.h"

namespace Ui {
class CParameterUserUI;
}

class CLIENT_EXPORT CParameterUserUI : public QWidget
{
    Q_OBJECT

public:
    explicit CParameterUserUI(QWidget *parent = nullptr);
    ~CParameterUserUI();
    
    //! [Parameter commone functions]
    /*!
     * \~chinese 设置参数，并初始化界面
     * \~english Set the parameters and initialize the user interface
     */
    int SetParameter(CParameterUser* pParameter);
    /*!
     * \~chinese 接受参数
     * \~english Accept parameters
     */
    int slotAccept();
    //! [Parameter commone functions]
    
private slots:
    void on_pbShow_clicked();
    void on_pbSave_clicked();
    
    void on_pbBrowsePublicFile_clicked();
    void on_pbBrowsePrivateFile_clicked();
    void on_pbShowPassphrase_clicked();
    void on_pbSavePassphrase_clicked();
    
    void on_cbType_currentIndexChanged(int index);
    
    void on_cbSystemFile_stateChanged(int arg1);
    
private:
    Ui::CParameterUserUI *ui;
    CParameterUser* m_pUser;
};

#endif // PARAMETERUSERUI_H
