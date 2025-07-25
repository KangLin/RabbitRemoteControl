// Author: Kang Lin <kl222@126.com>

#pragma once
#include <QWidget>
#include "Parameter.h"

/*!
 * \~chinese 参数界面接口
 * \~english The parameter UI interface
 * \~
 * \see CParameter
 * \ingroup CLIENT_PARAMETER CLIENT_PLUGIN_API
 */
class PLUGIN_EXPORT CParameterUI : public QWidget
{
    Q_OBJECT
public:
    explicit CParameterUI(QWidget *parent = nullptr);
    
    //! [override functions]

    /*!
     * \~chinese 设置参数，并初始化界面
     * \param pParameter: 相关的参数。在重载函数中转换成相应的类型。
     * \~english Set the parameters and initialize the user interface
     * \param pParameter: Related parameters. Convert to the appropriate type in the overloaded function.
     */
    virtual int SetParameter(CParameter* pParameter) = 0;
    /*!
     * \~chinese 检查参数的有效性
     * \param validity: 标志是否检查参数
     *    - true: 检查参数
     *    - false: 不检查参数
     * \~english Check parameters validity
     * \param validity
     *    - true: Check parameters
     *    - false: Not check parameters
     */
    virtual bool CheckValidity(bool validity = false);
    /*!
     * \~chinese 接受参数
     * \return 成功返回 0 。其它值为失败。
     * \~english Accept parameters
     * \return 0 is success. otherwise is fail
     */
    virtual int Accept() = 0;

    //! [override functions]

public Q_SLOTS:
    virtual void slotSetParameter(CParameter* pParameter);
    virtual void slotAccept();
};
