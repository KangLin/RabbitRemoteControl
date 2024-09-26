// Author: Kang Lin <kl222@126.com>

#ifndef PARAMETERWAKEONLANUI_H
#define PARAMETERWAKEONLANUI_H

#include <QWidget>
#include "ParameterWakeOnLan.h"

namespace Ui {
class CParameterWakeOnLanUI;
}

/*!
 * \brief The wake on lan parameters UI
 * \see CParameterWakeOnLan
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class CLIENT_EXPORT CParameterWakeOnLanUI : public QWidget
{
    Q_OBJECT

public:
    explicit CParameterWakeOnLanUI(QWidget *parent = nullptr);
    ~CParameterWakeOnLanUI();

    //! [Parameter commone functions]
    /*!
     * \~chinese 设置参数，并初始化界面
     * \~english Set the parameters and initialize the user interface
     */
    int SetParameter(CParameterWakeOnLan* pParameter);
    /*!
     * \~chinese 接受参数
     * \param validity: 标志是否检查参数
     *    - true: 检查参数
     *    - false: 不检查参数
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

private:
    Ui::CParameterWakeOnLanUI *ui;
    CParameterWakeOnLan* m_pWakeOnLan;
};

#endif // PARAMETERWAKEONLANUI_H
