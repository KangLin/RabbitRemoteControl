#ifndef PARAMETERUSERUI_H
#define PARAMETERUSERUI_H

#include <QWidget>
#include "ParameterUser.h"

namespace Ui {
class CParameterUserUI;
}

/*!
 * \brief The user parameters UI
 * \see CParameterUser
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
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
     * \param validity: 标志是否检查参数
     *    - true: 检查参数
     *    - false: 不检查参数
     * \return 成功返回 0 。其它值为失败。
     * \~english Accept parameters
     * \param validity
     *    - true: Check parameters
     *    - false: Not check parameters
     * \return 0 is success. otherwise is fail
     */
    int slotAccept(bool validity = false);
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
    
    void on_pbBrowseCAFile_clicked();
    
    void on_pbBrowseCRLFile_clicked();
    
private:
    Ui::CParameterUserUI *ui;
    CParameterUser* m_pUser;
};

#endif // PARAMETERUSERUI_H
