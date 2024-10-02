// Author: Kang Lin <kl222@126.com>

#ifndef PARAMETERWAKEONLANUI_H
#define PARAMETERWAKEONLANUI_H

#include <QWidget>
#include "ParameterWakeOnLan.h"
#include "ParameterUI.h"

namespace Ui {
class CParameterWakeOnLanUI;
}

/*!
 * \brief The wake on lan parameters UI
 * \see CParameterWakeOnLan
 * \ingroup CLIENT_PARAMETER_COMPONE
 */
class CLIENT_EXPORT CParameterWakeOnLanUI : public CParameterUI
{
    Q_OBJECT

public:
    explicit CParameterWakeOnLanUI(QWidget *parent = nullptr);
    ~CParameterWakeOnLanUI();

    /*!
     * \~chinese 设置参数，并初始化界面
     * \~english Set the parameters and initialize the user interface
     */
    int SetParameter(CParameter* pParameter);
    bool CheckValidity(bool validity);
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
    int Accept();

public Q_SLOTS:
    void slotHostChanged(const QString& szHost);
private slots:
    void on_pbShow_clicked();
    void on_pbSave_clicked();
    void on_cbNetworkInterface_currentIndexChanged(int index);

private:
    QString GetSubNet(const QString& szIP, const QString& szMask);

private:
    Ui::CParameterWakeOnLanUI *ui;
    CParameterWakeOnLan* m_pWakeOnLan;
};

#endif // PARAMETERWAKEONLANUI_H
