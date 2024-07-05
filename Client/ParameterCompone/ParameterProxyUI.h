#ifndef CPARAMETERPROXYUI_H
#define CPARAMETERPROXYUI_H

#include <QLabel>
#include <QComboBox>
#include "ParameterProxy.h"
#include "ParameterNetUI.h"

class CLIENT_EXPORT CParameterProxyUI : public QWidget
{
    Q_OBJECT
public:
    explicit CParameterProxyUI(QWidget *parent = nullptr);

    int SetParameter(CParameterProxy* pParameter);
    int slotAccept();
    
private Q_SLOTS:
    void slotTypeChanged(int nIndex);
    
private:
    QLabel* m_lbType;
    QComboBox* m_cbType;
    
    CParameterNetUI* m_uiSocket5;
    
    CParameterProxy* m_Proxy;
};

#endif // CPARAMETERPROXYUI_H
