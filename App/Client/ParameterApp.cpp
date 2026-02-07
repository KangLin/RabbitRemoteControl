#include "ParameterApp.h"
#include <QSettings>
#include <QDir>
#include "RabbitCommonDir.h"
#ifdef HAVE_ICE
    #include "Ice.h"
#endif

CParameterApp::CParameterApp(QObject *parent) : CParameter(parent, "MainWindow"),
    m_bReceiveShortCut(false),
    m_bSaveMainWindowStatus(true),
    m_ViewType(ViewType::Tab),
    m_TabPosition(QTabWidget::North),
    m_bEnableTabToolTip(true),
    m_bEnableTabIcon(true),
    m_bTabElided(false),
    m_nRecentMenuMaxCount(10),
    m_SystemTrayIconType(SystemTrayIconMenuType::MenuBar),
    m_bEnableSystemTrayIcon(true),
    m_bOpenLasterClose(false),
    m_bFavoriteEdit(false),
    m_bStatusBar(true),
    m_bTabBar(true),
    m_bMenuBar(true),
    m_bMessageBoxDisplayInfomation(true),
    m_bDockListActiveShowToolBar(true),
    m_bDockListRecentShowToolBar(true),
    m_bDockListFavoriteShowToolBar(true),
    m_bKeepSplitViewWhenFullScreen(false),
    m_bStartByType(true)
{
}

CParameterApp::~CParameterApp()
{
}

int CParameterApp::OnLoad(QSettings &set)
{
    SetReceiveShortCut(set.value("ReceiveShortCurt",
                                 GetReceiveShortCut()).toBool());
    
    set.beginGroup("Status");
    SetSaveMainWindowStatus(set.value("Enable",
                                      GetSaveMainWindowStatus()).toBool());
    if(GetSaveMainWindowStatus()) {
        SetStatusBar(set.value("StatusBar",
                               GetStatusBar()).toBool());
        SetTabBar(set.value("TabBar",
                            GetTabBar()).toBool());
        SetMenuBar(set.value("MenuBar",
                             GetMenuBar()).toBool());
    }
    set.endGroup();
    
    set.beginGroup("View");
    int viewType = set.value("Type", (int)GetViewType()).toInt();
    SetViewType((ViewType)viewType);
    SetTabPosition(static_cast<QTabWidget::TabPosition>(
                       set.value("TabView/Tab/Position",
                                 GetTabPosition()).toInt()));
    SetEnableTabToolTip(set.value("TabView/Tab/Enable/ToolTip",
                                  GetEnableTabToolTip()).toBool());
    SetEnableTabIcon(set.value("TabView/Tab/Enable/Icon",
                               GetEnableTabIcon()).toBool());
    SetTabElided(set.value("TabView/Tab/Enable/Elided", GetTabElided()).toBool());
    set.endGroup();

    SetRecentMenuMaxCount(set.value("Recent/Max",
                                    GetRecentMenuMaxCount()).toInt());

    set.beginGroup("SystemTrayIcon");
    //NOTE: The order cannot be changed
    SetEnableSystemTrayIcon(set.value("Enable",
                                      GetEnableSystemTrayIcon()).toBool());
    SetSystemTrayIconMenuType(static_cast<SystemTrayIconMenuType>(
                              set.value("MenuType",
                       static_cast<int>(GetSystemTrayIconMenuType())).toInt()));
    set.endGroup();

    SetOpenLasterClose(set.value("OpenLasterClose",
                                  GetOpenLasterClose()).toBool());
    SetFavoriteEdit(set.value("Favorite/Double/Edit",
                              GetFavoriteEdit()).toBool());

    SetMessageBoxDisplayInformation(
        set.value("MessageBoxDisplayInformation",
                  GetMessageBoxDisplayInformation()).toBool());

    set.beginGroup("Dock");
    SetDockListActiveShowToolBar(
        set.value("ListActive/ToolBar/Show",
                  GetDockListActiveShowToolBar()).toBool());

    SetDockListRecentShowToolBar(
        set.value("Recent/ToolBar/Show",
                  GetDockListRecentShowToolBar()).toBool());

    SetDockListFovoriteShowToolBar(
        set.value("Favorite/ToolBar/Show",
                  GetDockListFavoriteShowToolBar()).toBool());

    SetKeepSplitViewWhenFullScreen(
        set.value("KeepSplitViewWhenFullScreen",
                  GetKeepSplitViewWhenFullScreen()).toBool());

    SetStartByType(set.value("Menu/StartByType", GetStartByType()).toBool());

    set.endGroup();

#ifdef HAVE_ICE
    return CICE::Instance()->GetParameter()->Load(set);
#endif
    return 0;
}

int CParameterApp::OnSave(QSettings &set)
{
    set.setValue("ReceiveShortCurt", GetReceiveShortCut());

    set.beginGroup("Status");
    set.setValue("Enable", GetSaveMainWindowStatus());
    if(GetSaveMainWindowStatus())
    {
        set.setValue("StatusBar", GetStatusBar());
        set.setValue("TabBar", GetTabBar());
        set.setValue("MenuBar", GetMenuBar());
        // Geometry and status is saved MainWindow::closeEvent()
    }
    set.endGroup();

    set.beginGroup("View");
    set.setValue("Type", (int)GetViewType());
    set.setValue("TabView/Tab/Position", GetTabPosition());
    set.setValue("TabView/Tab/Enable/ToolTip", GetEnableTabToolTip());
    set.setValue("TabView/Tab/Enable/Icon", GetEnableTabIcon());
    set.setValue("TabView/Tab/Enable/Elided", GetTabElided());
    set.endGroup();

    set.setValue("Recent/Max", GetRecentMenuMaxCount());

    set.beginGroup("SystemTrayIcon");
    set.setValue("Enable", GetEnableSystemTrayIcon());
    set.setValue("MenuType",
                 static_cast<int>(GetSystemTrayIconMenuType()));
    set.endGroup();

    set.setValue("OpenLasterClose", GetOpenLasterClose());
    set.setValue("Favorite/Double/Edit", GetFavoriteEdit());
    set.setValue("MessageBoxDisplayInformation",
                 GetMessageBoxDisplayInformation());

    set.beginGroup("Dock");
    set.setValue("ListActive/ToolBar/Show",
                 GetDockListActiveShowToolBar());
    set.setValue("Recent/ToolBar/Show",
                 GetDockListRecentShowToolBar());
    set.setValue("Favorite/ToolBar/Show",
              GetDockListFavoriteShowToolBar());

    set.setValue("KeepSplitViewWhenFullScreen",
                 GetKeepSplitViewWhenFullScreen());

    set.setValue("Menu/StartByType", GetStartByType());

    set.endGroup();

#ifdef HAVE_ICE
    return CICE::Instance()->GetParameter()->Save(set);
#endif
    return 0;
}

bool CParameterApp::GetReceiveShortCut() const
{
    return m_bReceiveShortCut;
}

void CParameterApp::SetReceiveShortCut(bool newReceiveShortCut)
{
    if (m_bReceiveShortCut == newReceiveShortCut)
        return;
    m_bReceiveShortCut = newReceiveShortCut;
    SetModified(true);
    emit sigReceiveShortCutChanged();
}

bool CParameterApp::GetSaveMainWindowStatus() const
{
    return m_bSaveMainWindowStatus;
}

void CParameterApp::SetSaveMainWindowStatus(bool newSaveMainWindowStatus)
{
    if (m_bSaveMainWindowStatus == newSaveMainWindowStatus)
        return;
    m_bSaveMainWindowStatus = newSaveMainWindowStatus;
    SetModified(true);
    emit sigSaveMainWindowStatusChanged();
}

CParameterApp::ViewType CParameterApp::GetViewType()
{
    return m_ViewType;
}

int CParameterApp::SetViewType(ViewType type)
{
    if(m_ViewType == type)
        return 0;
    m_ViewType = type;
    SetModified(true);
    emit sigViewTypeChanged();
    return 0;
}

const QTabWidget::TabPosition &CParameterApp::GetTabPosition() const
{
    return m_TabPosition;
}

void CParameterApp::SetTabPosition(const QTabWidget::TabPosition &newTabPosition)
{
    if (m_TabPosition == newTabPosition)
        return;
    m_TabPosition = newTabPosition;
    SetModified(true);
    emit sigTabPositionChanged();
}

const bool CParameterApp::GetEnableTabToolTip() const
{
    return m_bEnableTabToolTip;
}

void CParameterApp::SetEnableTabToolTip(bool bEnable)
{
    if(m_bEnableTabToolTip == bEnable)
        return;
    m_bEnableTabToolTip = bEnable;
    SetModified(true);
    emit sigEnableTabToolTipChanged();
}

const bool CParameterApp::GetEnableTabIcon() const
{
    return m_bEnableTabIcon;
}

void CParameterApp::SetEnableTabIcon(bool bEnable)
{
    if(m_bEnableTabIcon == bEnable)
        return;
    m_bEnableTabIcon = bEnable;
    SetModified(true);
    emit sigEnableTabIconChanged();
}

const bool CParameterApp::GetTabElided() const
{
    return m_bTabElided;
}

void CParameterApp::SetTabElided(bool bElided)
{
    if(m_bTabElided == bElided)
        return;
    m_bTabElided = bElided;
    SetModified(true);
    emit sigTabElided();
}

int CParameterApp::GetRecentMenuMaxCount() const
{
    return m_nRecentMenuMaxCount;
}

void CParameterApp::SetRecentMenuMaxCount(int newRecentMenuMaxCount)
{
    if (m_nRecentMenuMaxCount == newRecentMenuMaxCount)
        return;
    m_nRecentMenuMaxCount = newRecentMenuMaxCount;
    SetModified(true);
    emit sigRecentMenuMaxCountChanged(m_nRecentMenuMaxCount);
}

CParameterApp::SystemTrayIconMenuType CParameterApp::GetSystemTrayIconMenuType() const
{
    return m_SystemTrayIconType;
}

void CParameterApp::SetSystemTrayIconMenuType(SystemTrayIconMenuType newSystemTrayIconType)
{
    if(m_SystemTrayIconType == newSystemTrayIconType)
        return;
    m_SystemTrayIconType = newSystemTrayIconType;
    SetModified(true);
    emit sigSystemTrayIconTypeChanged();
}

bool CParameterApp::GetEnableSystemTrayIcon() const
{
    return m_bEnableSystemTrayIcon;
}

int CParameterApp::SetEnableSystemTrayIcon(bool bShow)
{
    if(m_bEnableSystemTrayIcon == bShow)
        return 0;
    m_bEnableSystemTrayIcon = bShow;
    SetModified(true);
    emit sigEnableSystemTrayIcon();
    return 0;
}

bool CParameterApp::GetOpenLasterClose() const
{
    return m_bOpenLasterClose;
}

void CParameterApp::SetOpenLasterClose(bool newOpenLasterClose)
{
    if (m_bOpenLasterClose == newOpenLasterClose)
        return;
    m_bOpenLasterClose = newOpenLasterClose;
    SetModified(true);
    emit sigOpenLasterCloseChanged(m_bOpenLasterClose);
}

bool CParameterApp::GetFavoriteEdit() const
{
    return m_bFavoriteEdit;
}

void CParameterApp::SetFavoriteEdit(bool newFavoriteEdit)
{
    if(m_bFavoriteEdit == newFavoriteEdit)
        return;
    m_bFavoriteEdit = newFavoriteEdit;
    SetModified(true);
    emit sigFavoriteEditChanged(m_bFavoriteEdit);
}

bool CParameterApp::GetStatusBar() const
{
    return m_bStatusBar;
}

void CParameterApp::SetStatusBar(bool checked)
{
    if(m_bStatusBar == checked)
        return;
    m_bStatusBar = checked;
    SetModified(true);
}

bool CParameterApp::GetTabBar() const
{
    return m_bTabBar;
}

void CParameterApp::SetTabBar(bool checked)
{
    if(m_bTabBar == checked)
        return;
    m_bTabBar = checked;
    SetModified(true);
}

bool CParameterApp::GetMenuBar() const
{
    return m_bMenuBar;
}

void CParameterApp::SetMenuBar(bool checked)
{
    if(m_bMenuBar == checked)
        return;
    m_bMenuBar = checked;
    SetModified(true);
}

const bool CParameterApp::GetMessageBoxDisplayInformation() const
{
    return m_bMessageBoxDisplayInfomation;
}

void CParameterApp::SetMessageBoxDisplayInformation(bool bEnable)
{
    if(m_bMessageBoxDisplayInfomation == bEnable)
        return;
    m_bMessageBoxDisplayInfomation = bEnable;
    SetModified(true);
}

const bool CParameterApp::GetDockListActiveShowToolBar() const
{
    return m_bDockListActiveShowToolBar;
}

void CParameterApp::SetDockListActiveShowToolBar(bool bEnable)
{
    if(m_bDockListActiveShowToolBar == bEnable)
        return;
    m_bDockListActiveShowToolBar = bEnable;
    SetModified(true);
}

const bool CParameterApp::GetDockListRecentShowToolBar() const
{
    return m_bDockListRecentShowToolBar;
}

void CParameterApp::SetDockListRecentShowToolBar(bool bEnable)
{
    if(m_bDockListRecentShowToolBar == bEnable)
        return;
    m_bDockListRecentShowToolBar = bEnable;
    SetModified(true);
}

const bool CParameterApp::GetDockListFavoriteShowToolBar() const
{
    return m_bDockListFavoriteShowToolBar;
}

void CParameterApp::SetDockListFovoriteShowToolBar(bool bEnable)
{
    if(m_bDockListFavoriteShowToolBar == bEnable)
        return;
    m_bDockListFavoriteShowToolBar = bEnable;
    SetModified(true);
}

bool CParameterApp::GetKeepSplitViewWhenFullScreen() const
{
    return m_bKeepSplitViewWhenFullScreen;
}

void CParameterApp::SetKeepSplitViewWhenFullScreen(bool newKeepSplitViewWhenFullScreen)
{
    if(m_bKeepSplitViewWhenFullScreen == newKeepSplitViewWhenFullScreen)
        return;
    m_bKeepSplitViewWhenFullScreen = newKeepSplitViewWhenFullScreen;
    SetModified(true);
}

bool CParameterApp::GetStartByType() const
{
    return m_bStartByType;
}

void CParameterApp::SetStartByType(bool newStartByType)
{
    if (m_bStartByType == newStartByType)
        return;
    m_bStartByType = newStartByType;
    emit sigStartByTypeChanged();
}
