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
    
Q_SIGNALS:
    void sigSave();

private:
    int Init();
    int Clean();
    
private:
    Ui::MainWindow *ui;
    
    CManagerPlugins m_Plugins;
    QList<CService*> m_Service;
};

#endif // MAINWINDOW_H
