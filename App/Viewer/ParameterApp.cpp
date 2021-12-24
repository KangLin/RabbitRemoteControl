#include "ParameterApp.h"
#include <QSettings>
#include "RabbitCommonDir.h"

CParameterApp::CParameterApp(QObject *parent) : QObject(parent),
    m_bScreenShot(true),
    m_ScreenShotEndAction(NoAction),
    m_bReceiveShortCut(false),
    m_bSaveMainWindowStatus(true)
{
}

int CParameterApp::Load()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    SetScreenShot(set.value("ShotScreen/IsShotScreen", GetScreenShot()).toBool());
    SetScreenShotEndAction(static_cast<ScreenShotEndAction>(set.value("ShotScreen/Action", GetScreenShotEndAction()).toInt()));
    
    SetReceiveShortCut(set.value("MainWindow/ReceiveShortCurt", GetReceiveShortCut()).toBool());
    SetSaveMainWindowStatus(set.value("MainWindow/Status/Enable", GetSaveMainWindowStatus()).toBool());
    return 0;
}

int CParameterApp::Save()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("ShotScreen/IsShotScreen", GetScreenShot());
    set.setValue("ShotScreen/Action", GetScreenShotEndAction());
    set.setValue("MainWindow/ReceiveShortCurt", GetReceiveShortCut());
    set.setValue("MainWindow/Status/Enable", GetSaveMainWindowStatus());
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
