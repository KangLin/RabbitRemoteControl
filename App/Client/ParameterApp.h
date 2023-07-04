#ifndef CPARAMETERAPP_H
#define CPARAMETERAPP_H

#include <QObject>
#include <QTabWidget>

class CParameterApp : public QObject
{
    Q_OBJECT

public:
    explicit CParameterApp(QObject *parent = nullptr);

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
        Favorite,
        Remote,
        View,
        Tools
    };
    Q_ENUM(SystemTrayIconMenuType)
    SystemTrayIconMenuType GetSystemTrayIconMenuType() const;
    void SetSystemTrayIconMenuType(SystemTrayIconMenuType newSystemTrayIconType);
    bool GetEnableSystemTrayIcon() const;
    int SetEnableSystemTrayIcon(bool bShow);
Q_SIGNALS:
    void sigSystemTrayIconTypeChanged();
    void sigShowSystemTrayIcon();
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
};

#endif // CPARAMETERAPP_H
