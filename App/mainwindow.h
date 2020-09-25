#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QScrollArea>
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;
    
private slots:
    void on_actionAbout_A_triggered();
    void on_actionUpdate_U_triggered();
    void on_actionStatusBar_S_triggered();
    void on_actionToolBar_T_triggered();
    void on_actionFull_screen_F_triggered();
    void on_actionOriginal_O_triggered();
    void on_actionZoom_Z_triggered();
    void on_actionKeep_AspectRation_K_triggered();
    void on_actionExit_E_triggered();
    
    void on_actionConnect_C_triggered();
    
    void on_actionDisconnect_D_triggered();
    
protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    
private:
    Ui::MainWindow *ui;
    QScrollArea* m_pView;
    
};
#endif // MAINWINDOW_H
