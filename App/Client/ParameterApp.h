#ifndef CPARAMETERAPP_H
#define CPARAMETERAPP_H

#include <QTabWidget>
#include "Parameter.h"

class CParameterApp : public CParameter
{
    Q_OBJECT

public:
    explicit CParameterApp(QObject *parent = nullptr);
    virtual ~CParameterApp();

protected:
    virtual int OnLoad(QSettings &set);
    virtual int OnSave(QSettings &set);

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
    enum class ViewType
    {
        Tab,
        Splitter
    };
    Q_ENUM(ViewType)
    ViewType GetViewType();
    int SetViewType(ViewType type);
Q_SIGNALS:
    void sigViewTypeChanged();
private:
    ViewType m_ViewType;

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
    const bool GetTabElided() const;
    void SetTabElided(bool bElided);
Q_SIGNALS:
    void sigTabElided();
private:
    bool m_bTabElided;

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
        Operate,
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

public:
    const bool GetDockListActiveShowToolBar() const;
    void SetDockListActiveShowToolBar(bool bEnable);
private:
    bool m_bDockListActiveShowToolBar;

public:
    const bool GetDockListRecentShowToolBar() const;
    void SetDockListRecentShowToolBar(bool bEnable);
private:
    bool m_bDockListRecentShowToolBar;

public:
    bool GetKeepSplitViewWhenFullScreen() const;
    void SetKeepSplitViewWhenFullScreen(bool newKeepSplitViewWhenFullScreen);
    
private:
    bool m_bKeepSplitViewWhenFullScreen;
    
public:
    bool GetStartByType() const;
    void SetStartByType(bool newStartByType);
Q_SIGNALS:
    void sigStartByTypeChanged();    
private:
    bool m_bStartByType;
    Q_PROPERTY(bool m_bStartByType READ GetStartByType WRITE SetStartByType NOTIFY sigStartByTypeChanged FINAL)

};

#endif // CPARAMETERAPP_H
