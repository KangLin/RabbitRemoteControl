// Author: Kang Lin <kl222@126.com>

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTabWidget>
#include <QActionGroup>
#include <QToolButton>
#include <QSpinBox>
#include <QDockWidget>
#include <QSystemTrayIcon>
#include <QLabel>

#include "View.h"
#include "Manager.h"
#include "RabbitRecentMenu.h"
#include "ParameterApp.h"
#include "FavoriteView.h"
#include "FrmActive.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CFrmFullScreenToolBar;

/*!
 * \brief The MainWindow class
 * \ingroup VIEWER_APP
 */
class MainWindow : public QMainWindow, CManager::Handle
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
    void on_actionAbout_triggered();
    void on_actionUpdate_triggered();

    ///////// Start ///////// 
    void slotUpdateParameters(COperate* pOperate);
    void on_actionClone_triggered();
    void on_actionOpenRRCFile_triggered();
    void slotOpenFile(const QString& szFile, bool bOpenSettings = false);
    void slotStart();
    void slotRunning();
private:
    int Start(COperate* pOperate, bool set, QString szFile = QString());
    int LoadOperateLasterClose();
    int SaveOperateLasterClose();
public:
    virtual int onProcess(const QString &id, CPlugin *pPlugin) override;
    [[nodiscard]] QAction* GetStartAction(QMenu *pMenu, CPlugin* pPlug);
private Q_SLOTS:
    void slotStartByType();
private:
    CManager m_Manager;
    QVector<COperate*> m_Operates;
    QMap<CPlugin::TYPE, QMenu*> m_MenuStartByType;

private Q_SLOTS:
    void slotMenuActivity();
private:
    QActionGroup* m_pMenuActivityGroup;
    QToolButton* m_ptbMenuActivity;
private Q_SLOTS:
    void slotLoadOperateMenu();
    void slotCustomContextMenuRequested(const QPoint &pos);
Q_SIGNALS:
    void sigOperateMenuChanged(QAction* pAction);

public:
    QAction* m_pActionOperateMenu;
    QAction* m_pActionTBOperate;
    QToolButton* m_pTBOperate;

    ///////// Stop /////////
private Q_SLOTS:
    void on_actionStop_triggered();
    void slotCloseView(const QWidget* pView);
    void slotStop();
    void slotFinished();
    void slotError(const int nError, const QString &szInfo);
    /*!
     * \~chinese
     * \brief 用消息对话框(QMessageBox)显示信息
     * 
     * \~english
     * \brief Use message box display information
     *
     * \~
     * \see COperate::sigShowMessageBox()
     */
    virtual void slotShowMessageBox(const QString& title, const QString& message,
                                 const QMessageBox::Icon& icon);
    /*!
     * \~chinese 显示信息。暂时在状态栏显示。
     * \~english
     * \brief Show information
     * \param szInfo
     * \~
     * \see COperate::sigInformation()
     */
    void slotInformation(const QString& szInfo);
    void slotUpdateName(const QString& szName);
    void slotUpdateName();

    void on_actionSettings_triggered();
    void slotShortCut();

    ///////// Status bar //////////
private:
    enum class MessageLevel {
        Normal,
        Error,
        Warning
    };
private Q_SLOTS:
    void slotStatusMessage(QString szMessage, MessageLevel level = MessageLevel::Normal);
private:
    QLabel m_StatusBarMessage;

    ///////// Secure level /////////
private:
    QLabel* m_pSecureLevel;
    void SetSecureLevel(COperate *o);

    ///////// ICE signal /////////
private:
    QPushButton* m_pSignalStatus;
private Q_SLOTS:
    void slotSignalConnected();
    void slotSignalDisconnected();
    void slotSignalError(const int nError, const QString &szInfo);
    void slotSignalPushButtonClicked(bool checked);

protected:
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

    ///////// UI /////////
private:
    Ui::MainWindow *ui;
    CView* m_pView;

    void EnableMenu(bool bEnable);

private Q_SLOTS:
    void on_actionTabBar_B_toggled(bool bShow);
    void on_actionMain_menu_bar_M_toggled(bool checked);
    void on_actionToolBar_T_toggled(bool checked);
    void on_actionStatus_bar_S_toggled(bool checked);

    ///////// Full screen /////////
private:
    struct _FullState {
        bool statusbar;
        bool toolBar;
        bool menubar;
        bool dockListRecent;
        bool dockListActive;
        bool dockFavorite;
        bool dockDebugLog;
    };
    struct _FullState m_FullState;
    CFrmFullScreenToolBar* m_pFullScreenToolBar;
    friend CFrmFullScreenToolBar;
private Q_SLOTS:
    void on_actionFull_screen_F_triggered();

    ///////// View /////////
private Q_SLOTS:
    void slotCurrentViewChanged(const QWidget* pView);
    void slotViewerFocusIn(QWidget* pView);
    void slotOperateChanged(COperate* o);
    void on_actionViewTab_triggered();
    void on_actionViewSplit_triggered();
private:
    void SetView(CView* pView);

    ///////// Recent open /////////
private:
    RabbitCommon::CRecentMenu* m_pRecentMenu;

    ///////// List connects /////////
private:
    QDockWidget* m_pDockListRecent;
private Q_SLOTS:
    void on_actionOpenListRecent_triggered();
    
private:
    QDockWidget* m_pDockActive;
    CFrmActive* m_pFrmActive;

    ///////// Favorite //////////
private:
    QDockWidget* m_pDockFavorite;
    CFavoriteView* m_pFavoriteView;
private Q_SLOTS:
    void on_actionAdd_to_favorite_triggered();

private:
    CParameterApp m_Parameter;

    ///////// Drop //////////
protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dragMoveEvent(QDragMoveEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
private:
    QPoint m_DragStartPosition;

    ///////// System tray icon //////////
private Q_SLOTS:
    void slotSystemTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void slotSystemTrayIconTypeChanged();
    void slotEnableSystemTrayIcon();

private:
    QSharedPointer<QSystemTrayIcon> m_TrayIcon;
};

#endif // MAINWINDOW_H
