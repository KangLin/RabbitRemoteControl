#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ManagerPlugins.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
private slots:
    void on_pbCancel_clicked();
    void on_pbSave_clicked();
    
    void on_actionStart_triggered();
    
    void on_actionAbout_triggered();
    
    void on_actionDefault_triggered();
    
    void on_actionOpen_triggered();
    
Q_SIGNALS:
    void sigSave();

private:
    int InitTab();
    int Clean();
    int SetStatusText(QString szText);
    
private:
    Ui::MainWindow *ui;
    
    CManagerPlugins m_Plugins;
    QList<CService*> m_Service;
    bool m_bStart;
};

#endif // MAINWINDOW_H
