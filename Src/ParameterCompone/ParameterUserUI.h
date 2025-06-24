#ifndef PARAMETERUSERUI_H
#define PARAMETERUSERUI_H

#include <QWidget>
#include "ParameterUser.h"
#include "ParameterUI.h"

namespace Ui {
class CParameterUserUI;
}

/*!
 * \brief The user parameters UI
 * \see CParameterUser
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class PLUGIN_EXPORT CParameterUserUI : public CParameterUI
{
    Q_OBJECT

public:
    explicit CParameterUserUI(QWidget *parent = nullptr);
    virtual ~CParameterUserUI();
    
    //! [Parameter override functions]
    
    /*!
     * \~chinese 设置参数，并初始化界面
     * \~english Set the parameters and initialize the user interface
     */
    int SetParameter(CParameter* pParameter) override;
    /*!
     * \~chinese 接受参数
     * \return 成功返回 0 。其它值为失败。
     * \~english Accept parameters
     * \return 0 is success. otherwise is fail
     */
    int Accept() override;

    //! [Parameter override functions]
    
private slots:
    void on_pbShow_clicked();
    void on_pbSave_clicked();
    
    void on_pbBrowsePublicFile_clicked();
    void on_pbBrowsePrivateFile_clicked();
    void on_pbShowPassphrase_clicked();
    void on_pbSavePassphrase_clicked();
    
    void on_cbType_currentIndexChanged(int index);
    
    void on_cbSystemFile_stateChanged(int arg1);
    
    void on_pbBrowseCAFile_clicked();
    
    void on_pbBrowseCRLFile_clicked();
    
private:
    Ui::CParameterUserUI *ui;
    CParameterUser* m_pUser;
};

#endif // PARAMETERUSERUI_H
