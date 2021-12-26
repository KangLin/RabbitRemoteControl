// Author: Kang Lin <kl222@126.com>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTabWidget>
#include <QActionGroup>
#include <QToolButton>
#include <QSpinBox>

#include "ViewTable.h"
#include "ManageConnecter.h"
#include "RabbitRecentMenu.h"
#include "ParameterApp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CFrmFullScreenToolBar;

class MainWindow : public QMainWindow, CManageConnecter::Handle
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

Q_SIGNALS:
    void sigFullScreen();
    void sigShowNormal();
    
private Q_SLOTS:
    void on_actionExit_E_triggered();
    void on_actionFull_screen_F_triggered();
    
    void on_actionAbout_A_triggered();
    void on_actionUpdate_U_triggered();
    void on_actionStatusBar_S_triggered();
    void on_actionToolBar_T_triggered();
    
    void on_actionOpenStyle_O_triggered();
    void on_actionDefaultStyle_D_triggered();

    void on_actionSend_ctl_alt_del_triggered();
    
    void on_actionZoom_window_to_remote_desktop_triggered();
    void on_actionOriginal_O_changed();
    void on_actionOriginal_O_triggered();
    void on_actionZoom_In_triggered();
    void on_actionZoom_Out_triggered();
    void slotZoomFactor(int v);
    void on_actionZoomToWindow_Z_triggered();
    void on_actionKeep_aspect_ration_to_windows_K_toggled(bool arg1);
    void slotAdaptWindows(const CFrmViewer::ADAPT_WINDOWS aw);
    
    void on_actionDisconnect_D_triggered();
    void slotCloseView(const QWidget* pView);
    
    void slotUpdateParameters(CConnecter* pConnecter);
    void slotRecentFileTriggered(const QString& szFile);
    void on_actionOpen_O_triggered();
    void slotConnect();

    void slotConnected();
    void slotDisconnected();
    void slotError(const int nError, const QString &szInfo);
    /*!
     * \brief Show information
     * \param szInfo
     * \see CConnecter::sigInformation()
     */
    void slotInformation(const QString& szInfo);
    void slotUpdateServerName(const QString& szName);
    
    void on_actionShow_TabBar_B_triggered();
    void slotShowTabBar(bool bShow);

    void on_actionScreenshot_triggered();
    void on_actionSettings_triggered();
    void on_actionCurrent_connect_parameters_triggered();
    void slotShortCut();

    void on_actionFavorites_triggered();
    void on_actionClone_triggered();
    
protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;
    int Connect(CConnecter* p, bool set, QString szFile);

public:
    virtual int onProcess(const QString &id, CPluginViewer *pFactory) override;
private:
    CManageConnecter m_ManageConnecter;
    QMap<CConnecter*, QString> m_ConfigureFiles;

private:
    Ui::MainWindow *ui;
    QActionGroup* m_pGBView;
    QToolButton* m_ptbZoom;
    QSpinBox* m_psbZoomFactor;
    RabbitCommon::CRecentMenu* m_pRecentMenu;
    CView* m_pView;
    QVector<CConnecter*> m_Connecters;
    CFrmFullScreenToolBar* m_pFullScreenToolBar;
    bool m_bFullScreen;
    friend CFrmFullScreenToolBar;
    
public:
    CParameterApp m_Parameter;
};

#endif // MAINWINDOW_H
