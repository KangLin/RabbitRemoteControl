#include "ParameterApp.h"

CParameterApp::CParameterApp(QObject *parent) : QObject(parent),
    m_bReceiveShortCut(false),
    m_bScreenSlot(true)
{
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

bool CParameterApp::GetScreenSlot() const
{
    return m_bScreenSlot;
}

void CParameterApp::SetScreenSlot(bool newScreenSlot)
{
    if (m_bScreenSlot == newScreenSlot)
        return;
    m_bScreenSlot = newScreenSlot;
    emit sigScreenSlotChanged();
}
