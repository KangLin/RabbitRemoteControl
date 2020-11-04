#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QScrollArea>
#include <QMainWindow>
#include <QKeyEvent>
#include <QTabWidget>
#include <QActionGroup>

#include "ManageConnecter.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CFrmFullScreenToolBar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

private Q_SLOTS:
    void on_actionExit_E_triggered();
    void on_actionFull_screen_F_triggered();
    void on_actionAbout_A_triggered();
    void on_actionUpdate_U_triggered();
    void on_actionStatusBar_S_triggered();
    void on_actionToolBar_T_triggered();
    void on_actionOriginal_O_toggled(bool arg1);
    void on_actionZoom_Z_toggled(bool arg1);
    void on_actionKeep_AspectRation_K_toggled(bool arg1);

    void on_actionConnect_C_triggered();
    void on_actionDisconnect_D_triggered();

    void slotCurrentChanged(int index);
    void slotTabCloseRequested(int index);

    void slotViewTitleChanged(const QString& szName);
    void slotConnected();
    void slotDisconnected();

protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
    QScrollArea* GetScrollArea(int index);
    CFrmViewer* GetViewer(int index);
    CConnecter* GetConnecter(int index);
    int GetViewIndex(CFrmViewer* pView);

private:
    Ui::MainWindow *ui;
    QActionGroup* m_pGBView;
    QTabWidget* m_pTab;
    CManageConnecter m_ManageConnecter;
    QMap<CFrmViewer*, CConnecter*> m_Connecters;
    CFrmFullScreenToolBar* m_pFullScreenToolBar;
};

#endif // MAINWINDOW_H
