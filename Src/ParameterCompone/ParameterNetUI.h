#ifndef PARAMETERNETUI_H
#define PARAMETERNETUI_H

#include <QWidget>
#include "ParameterNet.h"
#include "ParameterUI.h"

namespace Ui {
class CParameterNetUI;
}

/*!
 * \brief The net parameter UI
 * \see CParameterNet
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class PLUGIN_EXPORT CParameterNetUI : public CParameterUI
{
    Q_OBJECT

public:
    explicit CParameterNetUI(QWidget *parent = nullptr);
    ~CParameterNetUI();
    
    //! [Parameter commone functions]
    /*!
     * \~chinese 设置参数，并初始化界面
     * \~english Set the parameters and initialize the user interface
     */
    int SetParameter(CParameter* pParameter);
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
    int Accept();
    bool CheckValidity(bool validity = false);
    //! [Parameter commone functions]

Q_SIGNALS:
    void sigHostChanged(const QString& szHost);

private slots:
    void on_leHost_editingFinished();
    
private:
    Ui::CParameterNetUI *ui;
    CParameterNet* m_pNet;
};

#endif // PARAMETERNETUI_H
