#ifndef CPARAMETERAPP_H
#define CPARAMETERAPP_H

#include <QObject>
#include <QTabWidget>

class CParameterApp : public QObject
{
    Q_OBJECT

public:
    explicit CParameterApp(QObject *parent = nullptr);
    virtual ~CParameterApp();

public Q_SLOTS:
    virtual int Load();
    virtual int Save();
    
public:
    bool GetScreenShot() const;
    void SetScreenShot(bool newScreenShot);
    const QString &GetScreenShotPath() const;
    void SetScreenShotPath(const QString& path);
Q_SIGNALS:
    void sigScreenShotChanged();
private:
    bool m_bScreenShot;
    QString m_szScreenShotPath;
    Q_PROPERTY(bool ScreenShot READ GetScreenShot WRITE SetScreenShot NOTIFY sigScreenShotChanged)

public:
    enum ScreenShotEndAction {
        NoAction,
        OpenFolder,
        OpenFile
    };
    ScreenShotEndAction GetScreenShotEndAction() const;
    void SetScreenShotEndAction(ScreenShotEndAction newScreenShotEndAction);    
Q_SIGNALS:
    void sigScreenShotEndActionChanged();
private:
    ScreenShotEndAction m_ScreenShotEndAction;
    Q_PROPERTY(ScreenShotEndAction ScreenShotEndAction READ GetScreenShotEndAction WRITE SetScreenShotEndAction NOTIFY sigScreenShotEndActionChanged)
    
public:    
    bool GetReceiveShortCut() const;
    void SetReceiveShortCut(bool newReceiveShortCut);   
Q_SIGNALS:
    void sigReceiveShortCutChanged();  
private:
    bool m_bReceiveShortCut;
    Q_PROPERTY(bool ReceiveShortCut READ GetReceiveShortCut WRITE SetReceiveShortCut NOTIFY sigReceiveShortCutChanged)
    
public:
    bool GetSaveMainWindowStatus() const;
    void SetSaveMainWindowStatus(bool newSaveMainWindowStatus);
Q_SIGNALS:
    void sigSaveMainWindowStatusChanged();
private:
    bool m_bSaveMainWindowStatus;
    Q_PROPERTY(bool SaveMainWindowStatus READ GetSaveMainWindowStatus WRITE SetSaveMainWindowStatus NOTIFY sigSaveMainWindowStatusChanged)
    
public:
    const QTabWidget::TabPosition &GetTabPosition() const;
    void SetTabPosition(const QTabWidget::TabPosition &newTabPosition);
Q_SIGNALS:
    void sigTabPositionChanged();
private:    
    QTabWidget::TabPosition m_TabPosition;
    Q_PROPERTY(QTabWidget::TabPosition TabPosition READ GetTabPosition WRITE SetTabPosition NOTIFY sigTabPositionChanged)

public:
    const bool GetEnableTabToolTip() const;
    void SetEnableTabToolTip(bool bEnable);
Q_SIGNALS:
    void sigEnableTabToolTipChanged();
private:
    bool m_bEnableTabToolTip;
    Q_PROPERTY(bool EnableTabToolTip READ GetEnableTabToolTip WRITE SetEnableTabToolTip NOTIFY sigEnableTabToolTipChanged)

public:
    const bool GetEnableTabIcon() const;
    void SetEnableTabIcon(bool bEnable);
Q_SIGNALS:
    void sigEnableTabIconChanged();
private:
    bool m_bEnableTabIcon;
    Q_PROPERTY(bool EnableTabIcon READ GetEnableTabIcon WRITE SetEnableTabIcon NOTIFY sigEnableTabIconChanged)

public:
    int GetRecentMenuMaxCount() const;
    void SetRecentMenuMaxCount(int newRecentMenuMaxCount);
Q_SIGNALS:
    void sigRecentMenuMaxCountChanged(int);
private:
    int m_nRecentMenuMaxCount;
    Q_PROPERTY(int RecentMenuMaxCount READ GetRecentMenuMaxCount WRITE SetRecentMenuMaxCount NOTIFY sigRecentMenuMaxCountChanged)

public:
    enum class SystemTrayIconMenuType
    {
        No,
        RecentOpen,
        Remote,
        View,
        Tools,
        MenuBar
    };
    Q_ENUM(SystemTrayIconMenuType)
    SystemTrayIconMenuType GetSystemTrayIconMenuType() const;
    void SetSystemTrayIconMenuType(SystemTrayIconMenuType newSystemTrayIconType);
    bool GetEnableSystemTrayIcon() const;
    int SetEnableSystemTrayIcon(bool bShow);
Q_SIGNALS:
    void sigSystemTrayIconTypeChanged();
    void sigEnableSystemTrayIcon();
private:
    SystemTrayIconMenuType m_SystemTrayIconType;
    bool m_bEnableSystemTrayIcon;
    Q_PROPERTY(SystemTrayIconMenuType SystemTrayIconMenuType READ GetSystemTrayIconMenuType WRITE SetSystemTrayIconMenuType NOTIFY sigSystemTrayIconTypeChanged)
    
public:
    bool GetOpenLasterClose() const;
    void SetOpenLasterClose(bool newOpenLasterClose);
Q_SIGNALS:
    void sigOpenLasterCloseChanged(bool bOpenLasterClose);
private:
    bool m_bOpenLasterClose;
    Q_PROPERTY(bool bOpenLasterClose READ GetOpenLasterClose WRITE SetOpenLasterClose NOTIFY sigOpenLasterCloseChanged)
    
public:
    bool GetFavoriteEdit() const;
    void SetFavoriteEdit(bool newFavoriteEdit);
Q_SIGNALS:
    void sigFavoriteEditChanged(bool FavoriteEdit);
private:
    bool m_bFavoriteEdit;
    Q_PROPERTY(bool FavoriteEdit READ GetFavoriteEdit WRITE SetFavoriteEdit NOTIFY sigFavoriteEditChanged)
    
public:
    bool GetStatusBar() const;
    void SetStatusBar(bool checked);
private:
    bool m_bStatusBar;
    Q_PROPERTY(bool StatusBar READ GetStatusBar WRITE SetStatusBar)
    
public:
    bool GetTabBar() const;
    void SetTabBar(bool checked);
private:
    bool m_bTabBar;
    Q_PROPERTY(bool TabBar READ GetTabBar WRITE SetTabBar)
    
public:
    bool GetMenuBar() const;
    void SetMenuBar(bool checked);
private:
    bool m_bMenuBar;
    Q_PROPERTY(bool MenuBar READ GetMenuBar WRITE SetMenuBar)
    
public:
    const bool GetMessageBoxDisplayInformation() const;
    void SetMessageBoxDisplayInformation(bool bEnable);
private:
    bool m_bMessageBoxDisplayInfomation;
    Q_PROPERTY(bool MessageBoxDisplyInformtion READ GetMessageBoxDisplayInformation WRITE SetMessageBoxDisplayInformation)
};

#endif // CPARAMETERAPP_H
