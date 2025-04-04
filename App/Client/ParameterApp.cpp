#include "ParameterApp.h"
#include <QSettings>
#include <QDir>
#include "RabbitCommonDir.h"
#ifdef HAVE_ICE
    #include "Ice.h"
#endif

CParameterApp::CParameterApp(QObject *parent) : QObject(parent),
    m_bReceiveShortCut(false),
    m_bSaveMainWindowStatus(true),
    m_ViewType(ViewType::Tab),
    m_TabPosition(QTabWidget::North),
    m_bEnableTabToolTip(true),
    m_bEnableTabIcon(true),
    m_nRecentMenuMaxCount(10),
    m_SystemTrayIconType(SystemTrayIconMenuType::MenuBar),
    m_bEnableSystemTrayIcon(true),
    m_bOpenLasterClose(false),
    m_bFavoriteEdit(false),
    m_bStatusBar(true),
    m_bTabBar(true),
    m_bMenuBar(true),
    m_bMessageBoxDisplayInfomation(true),
    m_bDockListConnectersShowToolBar(true),
    m_bDockListRecentShowToolBar(true)
{
}

CParameterApp::~CParameterApp()
{
}

int CParameterApp::Load()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);

    SetReceiveShortCut(set.value("MainWindow/ReceiveShortCurt",
                                 GetReceiveShortCut()).toBool());
    SetSaveMainWindowStatus(set.value("MainWindow/Status/Enable",
                                      GetSaveMainWindowStatus()).toBool());
    if(GetSaveMainWindowStatus()) {
        SetStatusBar(set.value("MainWindow/Status/StatusBar",
                               GetStatusBar()).toBool());

        SetTabBar(set.value("MainWindow/Status/TabBar",
                            GetTabBar()).toBool());

        SetMenuBar(set.value("MainWindow/Status/MenuBar",
                             GetMenuBar()).toBool());
    }

    int viewType = set.value("MainWindow/View/Type").toInt();
    SetViewType((ViewType)viewType);
    SetTabPosition(static_cast<QTabWidget::TabPosition>(
                       set.value("MainWindow/View/TabView/Tab/Position",
                                 GetTabPosition()).toInt()));
    SetEnableTabToolTip(set.value("MainWindow/View/TabView/Tab/Enable/ToolTip",
                                  GetEnableTabToolTip()).toBool());
    SetEnableTabIcon(set.value("MainWindow/View/TabView/Tab/Enable/Icon",
                               GetEnableTabIcon()).toBool());

    SetRecentMenuMaxCount(set.value("MainWindow/Recent/Max",
                                    GetRecentMenuMaxCount()).toInt());
    
    //NOTE: The order cannot be changed
    SetEnableSystemTrayIcon(set.value("MainWindow/SystemTrayIcon/Enable",
                                      GetEnableSystemTrayIcon()).toBool());
    SetSystemTrayIconMenuType(static_cast<SystemTrayIconMenuType>(
                              set.value("MainWindow/SystemTrayIcon/MenuType",
                       static_cast<int>(GetSystemTrayIconMenuType())).toInt()));

    SetOpenLasterClose(set.value("MainWindow/OpenLasterClose",
                                  GetOpenLasterClose()).toBool());
    SetFavoriteEdit(set.value("MainWindow/Favorite/Double/Edit",
                              GetFavoriteEdit()).toBool());

    SetMessageBoxDisplayInformation(
        set.value("MainWindow/MessageBoxDisplayInformation",
                  GetMessageBoxDisplayInformation()).toBool());

    SetDockListConnectersShowToolBar(
        set.value("MainWindow/Dock/ListConnecters/ToolBar/Show",
                  GetDockListConnectersShowToolBar()).toBool());

    SetDockListRecentShowToolBar(
        set.value("MainWindow/Dock/Recent/ToolBar/Show",
                  GetDockListRecentShowToolBar()).toBool());

#ifdef HAVE_ICE
    return CICE::Instance()->GetParameter()->Load(set);
#endif
    return 0;
}

int CParameterApp::Save()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);

    set.setValue("MainWindow/ReceiveShortCurt", GetReceiveShortCut());
    set.setValue("MainWindow/Status/Enable", GetSaveMainWindowStatus());
    if(GetSaveMainWindowStatus())
    {
        set.setValue("MainWindow/Status/StatusBar", GetStatusBar());
        set.setValue("MainWindow/Status/TabBar", GetTabBar());
        set.setValue("MainWindow/Status/MenuBar", GetMenuBar());
        // Geometry and status is saved MainWindow::closeEvent()
    }
    set.setValue("MainWindow/View/Type", (int)GetViewType());
    set.setValue("MainWindow/View/TabView/Tab/Position", GetTabPosition());
    set.setValue("MainWindow/View/TabView/Tab/Enable/ToolTip", GetEnableTabToolTip());
    set.setValue("MainWindow/View/TabView/Tab/Enable/Icon", GetEnableTabIcon());
    set.setValue("MainWindow/Recent/Max", GetRecentMenuMaxCount());
    set.setValue("MainWindow/SystemTrayIcon/Enable", GetEnableSystemTrayIcon());
    set.setValue("MainWindow/SystemTrayIcon/MenuType",
                 static_cast<int>(GetSystemTrayIconMenuType()));
    set.setValue("MainWindow/OpenLasterClose", GetOpenLasterClose());
    set.setValue("MainWindow/Favorite/Double/Edit", GetFavoriteEdit());
    set.setValue("MainWindow/MessageBoxDisplayInformation",
                 GetMessageBoxDisplayInformation());
    set.setValue("MainWindow/Dock/ListConnecters/ToolBar/Show",
                 GetDockListConnectersShowToolBar());
    set.setValue("MainWindow/Dock/Recent/ToolBar/Show",
                 GetDockListRecentShowToolBar());

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
    emit sigEnableTabIconChanged();
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
    emit sigOpenLasterCloseChanged(m_bOpenLasterClose);
}

bool CParameterApp::GetFavoriteEdit() const
{
    return m_bFavoriteEdit;
}

void CParameterApp::SetFavoriteEdit(bool newFavoriteEdit)
{
    m_bFavoriteEdit = newFavoriteEdit;
    emit sigFavoriteEditChanged(m_bFavoriteEdit);
}

bool CParameterApp::GetStatusBar() const
{
    return m_bStatusBar;
}

void CParameterApp::SetStatusBar(bool checked)
{
    m_bStatusBar = checked;
}

bool CParameterApp::GetTabBar() const
{
    return m_bTabBar;
}

void CParameterApp::SetTabBar(bool checked)
{
    m_bTabBar = checked;
}

bool CParameterApp::GetMenuBar() const
{
    return m_bMenuBar;
}

void CParameterApp::SetMenuBar(bool checked)
{
    m_bMenuBar = checked;
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
}

const bool CParameterApp::GetDockListConnectersShowToolBar() const
{
    return m_bDockListConnectersShowToolBar;
}

void CParameterApp::SetDockListConnectersShowToolBar(bool bEnable)
{
    if(m_bDockListConnectersShowToolBar == bEnable)
        return;
    m_bDockListConnectersShowToolBar = bEnable;
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
}
