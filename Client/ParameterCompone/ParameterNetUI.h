#ifndef PARAMETERNETUI_H
#define PARAMETERNETUI_H

#include <QWidget>
#include "ParameterNet.h"

namespace Ui {
class CParameterNetUI;
}

class CLIENT_EXPORT CParameterNetUI : public QWidget
{
    Q_OBJECT

public:
    explicit CParameterNetUI(QWidget *parent = nullptr);
    ~CParameterNetUI();
    
    int SetParameter(CParameterNet* pParameter);
    int slotAccept();
    
private slots:
    void on_leHost_editingFinished();
    
private:
    Ui::CParameterNetUI *ui;
    CParameterNet* m_pNet;
};

#endif // PARAMETERNETUI_H
