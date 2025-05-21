#ifndef CPARAMETERPROXYUI_H
#define CPARAMETERPROXYUI_H

#include <QLabel>
#include <QComboBox>
#include "ParameterProxy.h"
#include "ParameterNetUI.h"
#include "ParameterSSHTunnelUI.h"

/*!
 * \brief The proxy parameter UI
 * \see CParameterProxy CLIENT_PARAMETER_COMPONE
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class CLIENT_EXPORT CParameterProxyUI : public CParameterUI
{
    Q_OBJECT
public:
    explicit CParameterProxyUI(QWidget *parent = nullptr);

    /*!
     * \~chinese 设置参数，并初始化界面
     * \~english Set the parameters and initialize the user interface
     */
    int SetParameter(CParameter* pParameter);
    bool CheckValidity(bool validity = false);
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
    
private Q_SLOTS:
    void slotTypeChanged(int nIndex);
    
private:
    QLabel* m_lbType;
    QComboBox* m_cbType;
    
    CParameterNetUI* m_uiSockesV5;
    CParameterSSHTunnelUI* m_uiSSH;
    
    CParameterProxy* m_Proxy;
};

#endif // CPARAMETERPROXYUI_H
