//! @author: Kang Lin(kl222@126.com)

#ifndef FRMFULLSCREENTOOLBAR_H
#define FRMFULLSCREENTOOLBAR_H

#include <QAction>
#include <QWidget>
#include <QMouseEvent>
#include <QPointF>
#include <QToolBar>
#include <QTimer>
#include "mainwindow.h"

namespace Ui {
class CFrmFullScreenToolBar;
}

class CFrmFullScreenToolBar : public QWidget
{
    Q_OBJECT
    
public:
    explicit CFrmFullScreenToolBar(MainWindow* pMain, QWidget *parent = nullptr);
    virtual ~CFrmFullScreenToolBar() override;
    
Q_SIGNALS:
    void sigExitFullScreen();
    void sigExit();
    void sigDisconnect();

private Q_SLOTS:
    void slotTimeOut();
    
protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    
private:
    int ReSize();
    
private:
    Ui::CFrmFullScreenToolBar *ui;
    
    QPointF m_Pos;
    QToolBar m_ToolBar;
    QAction* m_pNail;
    MainWindow* m_pMain;
    QTimer m_Timer;
    int m_TimeOut;
    bool m_isHide;
};

#endif // FRMFULLSCREENTOOLBAR_H
