// Author: Kang Lin <kl222@126.com>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ManagerPlugins.h"

namespace Ui {
class MainWindow;
}

/*!
 * \brief The Main window class
 * \ingroup ServiceConfigure
 */
class CMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit CMainWindow(QWidget *parent = nullptr);
    ~CMainWindow();

private Q_SLOTS:
    void on_pbCancel_clicked();
    void on_pbSave_clicked();
    void on_actionStart_triggered();
    void on_actionAbout_triggered();
    void on_actionOpen_folder_triggered();

    void on_actionStyle_S_triggered();

Q_SIGNALS:
    /*!
     * \~chinese 通知插件设置窗口参数发生改变。
     *           设置参数窗口必须有定义槽 \b slotAccept 。
     *           例如：CFrmParameterFreeRDP::slotAccept
     * \~english Notifies the plugin settings window that parameters have changed.
     *           the widget must has slot \b slotAccept .
     *           Eg: CFrmParameterFreeRDP::slotAccept
     * \~
     * \snippet Plugins/FreeRDP/Service/FrmParameterFreeRDP.h Accept parameters
     * \see CService::GetParameterWidget
     */
    void sigAccept();

private:
    int InitTab();
    int Clean();
    int SetStatusText(QString szText);
    
private:
    Ui::MainWindow *ui;
    
    CManagePlugins m_Plugins;
    QList<CService*> m_Service;
    bool m_bStart;
};

#endif // MAINWINDOW_H
