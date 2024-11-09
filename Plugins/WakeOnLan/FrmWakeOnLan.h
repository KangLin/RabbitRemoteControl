#ifndef FRMWAKEONLAN_H
#define FRMWAKEONLAN_H

#include <QWidget>

#include "WakeOnLanModel.h"

namespace Ui {
class CFrmWakeOnLan;
}

class CFrmWakeOnLan : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmWakeOnLan(CWakeOnLanModel* pModel,
                           QWidget *parent = nullptr);
    ~CFrmWakeOnLan();
    QModelIndex GetCurrentIndex();
public Q_SLOTS:
    void slotRemoveRow();
private:
    Ui::CFrmWakeOnLan *ui;
};

#endif // FRMWAKEONLAN_H
