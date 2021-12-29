#include "ParameterApp.h"
#include <QSettings>
#include "RabbitCommonDir.h"
#include "ManagePassword.h"

CParameterApp::CParameterApp(QObject *parent) : QObject(parent),
    m_bScreenShot(true),
    m_ScreenShotEndAction(NoAction),
    m_bReceiveShortCut(false),
    m_bSaveMainWindowStatus(true),
    m_TabPosition(QTabWidget::North),
    m_nRecentMenuMaxCount(10),
    m_SystemTrayIconType(SystemTrayIconMenuType::Remote)
{
}

int CParameterApp::Load()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    CManagePassword::Instance()->Load(set);
    
    SetScreenShot(set.value("ShotScreen/IsShotScreen",
                            GetScreenShot()).toBool());
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
    return 0;
}

int CParameterApp::Save()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    CManagePassword::Instance()->Save(set);

    set.setValue("ShotScreen/IsShotScreen", GetScreenShot());
    set.setValue("ShotScreen/Action", GetScreenShotEndAction());
    set.setValue("MainWindow/ReceiveShortCurt", GetReceiveShortCut());
    set.setValue("MainWindow/Status/Enable", GetSaveMainWindowStatus());
    set.setValue("MainWindow/Tab/Position", GetTabPosition());
    set.setValue("MainWindow/Recent/Max", GetRecentMenuMaxCount());
    set.setValue("MainWindow/SystemTrayIcon/MenuType",
                 static_cast<int>(GetSystemTrayIconMenuType()));
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
