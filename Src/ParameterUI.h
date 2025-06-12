#ifndef PARAMETERUI_H
#define PARAMETERUI_H

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

    /*!
     * \~chinese 设置参数，并初始化界面
     * \~english Set the parameters and initialize the user interface
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

public Q_SLOTS:
    virtual void slotSetParameter(CParameter* pParameter);
    virtual void slotAccept();
};

#endif // PARAMETERUI_H
