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
    QModelIndexList GetSelect();
public Q_SLOTS:
    void slotRemoveRow();
Q_SIGNALS:
    /*!
     * \~chinese 视图获得焦点
     * \~english The view is focus
     * \param pView
     */
    void sigViewerFocusIn(QWidget* pView);
private:
    Ui::CFrmWakeOnLan *ui;
    
    // QObject interface
public:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // FRMWAKEONLAN_H
