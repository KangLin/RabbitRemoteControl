#include "ParameterViewer.h"

CParameterViewer::CParameterViewer(QObject *parent)
    : CParameter(parent)
{
    m_bHookKeyboard = true;
}

CParameterViewer::~CParameterViewer()
{}

int CParameterViewer::Load(QSettings &set)
{
    SetHookKeyboard(set.value("Viewer/Hook/Keyboard",
                              GetHookKeyboard()).toBool());
    return 0;
}

int CParameterViewer::Save(QSettings& set)
{
    set.setValue("Viewer/Hook/Keyboard", GetHookKeyboard());
    return 0;
}

bool CParameterViewer::GetHookKeyboard() const
{
    return m_bHookKeyboard;
}

void CParameterViewer::SetHookKeyboard(bool newHookKeyboard)
{
    if (m_bHookKeyboard == newHookKeyboard)
        return;
    m_bHookKeyboard = newHookKeyboard;
    emit sigHookKeyboardChanged();
}
