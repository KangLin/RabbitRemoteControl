#include "ParameterApp.h"
#include <QSettings>
#include <QDir>
#include "RabbitCommonDir.h"
#include "ManagePassword.h"
#include "Ice.h"

CParameterApp::CParameterApp(QObject *parent) : QObject(parent),
    m_bScreenShot(false),
    m_ScreenShotEndAction(NoAction),
    m_bReceiveShortCut(false),
    m_bSaveMainWindowStatus(true),
    m_TabPosition(QTabWidget::North),
    m_nRecentMenuMaxCount(10),
    m_SystemTrayIconType(SystemTrayIconMenuType::Remote),
    m_bEnableSystemTrayIcon(true),
    m_bOpenLasterClose(false),
    m_bFavoriteEdit(false)
{
    m_szScreenShotPath = RabbitCommon::CDir::Instance()->GetDirUserImage()
            + QDir::separator()
            + "ScreenSlot";
}

int CParameterApp::Load()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    CManagePassword::Instance()->Load(set);
    
    SetScreenShot(set.value("ShotScreen/ShotScreen/Desktop",
                            GetScreenShot()).toBool());
    SetScreenShotPath(set.value("ShotScreen/ShotScreen/Path",
                                GetScreenShotPath()).toString());
    SetScreenShotEndAction(static_cast<ScreenShotEndAction>(
                               set.value("ShotScreen/Action",
                                         GetScreenShotEndAction()).toInt()));
    
    SetReceiveShortCut(set.value("MainWindow/ReceiveShortCurt",
                                 GetReceiveShortCut()).toBool());
    SetSaveMainWindowStatus(set.value("MainWindow/Status/Enable",
                                      GetSaveMainWindowStatus()).toBool());
    SetTabPosition(static_cast<QTabWidget::TabPosition>(
                       set.value("MainWindow/Tab/Position",
                                 GetTabPosition()).toInt()));

    SetRecentMenuMaxCount(set.value("MainWindow/Recent/Max",
                                    GetRecentMenuMaxCount()).toInt());

    SetSystemTrayIconMenuType(static_cast<SystemTrayIconMenuType>(
                              set.value("MainWindow/SystemTrayIcon/MenuType",
                       static_cast<int>(GetSystemTrayIconMenuType())).toInt()));
    SetEnableSystemTrayIcon(set.value("MainWindow/SystemTrayIcon/Enable",
                                    GetEnableSystemTrayIcon()).toBool());
    SetOpenLasterClose(set.value("MainWindow/OpenLasterClose",
                                  GetOpenLasterClose()).toBool());
    SetFavoriteEdit(set.value("MainWindow/Favorite/Double/Edit",
                              GetFavoriteEdit()).toBool());
    return CICE::Instance()->GetParameter()->Load(set);
}

int CParameterApp::Save()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    CManagePassword::Instance()->Save(set);

    set.setValue("ShotScreen/ShotScreen/Desktop", GetScreenShot());
    set.setValue("ShotScreen/ShotScreen/Path", GetScreenShotPath());
    set.setValue("ShotScreen/Action", GetScreenShotEndAction());
    set.setValue("MainWindow/ReceiveShortCurt", GetReceiveShortCut());
    set.setValue("MainWindow/Status/Enable", GetSaveMainWindowStatus());
    set.setValue("MainWindow/Tab/Position", GetTabPosition());
    set.setValue("MainWindow/Recent/Max", GetRecentMenuMaxCount());
    set.setValue("MainWindow/SystemTrayIcon/MenuType",
                 static_cast<int>(GetSystemTrayIconMenuType()));
    set.setValue("MainWindow/SystemTrayIcon/Enable", GetEnableSystemTrayIcon());
    set.setValue("MainWindow/OpenLasterClose", GetOpenLasterClose());
    set.setValue("MainWindow/Favorite/Double/Edit", GetFavoriteEdit());
    
    return CICE::Instance()->GetParameter()->Save(set);
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
    m_SystemTrayIconType = newSystemTrayIconType;
    emit sigSystemTrayIconTypeChanged();
}

bool CParameterApp::GetEnableSystemTrayIcon() const
{
    return m_bEnableSystemTrayIcon;
}

int CParameterApp::SetEnableSystemTrayIcon(bool bShow)
{
    m_bEnableSystemTrayIcon = bShow;
    emit sigShowSystemTrayIcon();
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

bool CParameterApp::GetScreenShot() const
{
    return m_bScreenShot;
}

void CParameterApp::SetScreenShot(bool newScreenShot)
{
    if (m_bScreenShot == newScreenShot)
        return;
    m_bScreenShot = newScreenShot;
    emit sigScreenShotChanged();
}

const QString& CParameterApp::GetScreenShotPath() const
{
    return m_szScreenShotPath;
}

void CParameterApp::SetScreenShotPath(const QString &path)
{
    m_szScreenShotPath = path;
}

CParameterApp::ScreenShotEndAction CParameterApp::GetScreenShotEndAction() const
{
    return m_ScreenShotEndAction;
}

void CParameterApp::SetScreenShotEndAction(ScreenShotEndAction newScreenShotEndAction)
{
    if (m_ScreenShotEndAction == newScreenShotEndAction)
        return;
    m_ScreenShotEndAction = newScreenShotEndAction;
    emit sigScreenShotEndActionChanged();
}
