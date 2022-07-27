#include "ParameterViewer.h"

CParameterViewer::CParameterViewer(QObject *parent)
    : CParameter(parent),
      m_bHookKeyboard(true),
      m_bSavePassword(false),
      m_PromptType(PromptType::First),
      m_nPromptCount(0),
      m_bViewPassowrd(false)
{}

CParameterViewer::~CParameterViewer()
{}

int CParameterViewer::Load(QSettings &set)
{
    SetHookKeyboard(set.value("Viewer/Hook/Keyboard",
                              GetHookKeyboard()).toBool());
    SetPromptType(static_cast<PromptType>(
                    set.value("Manage/Password/Prompty/Type",
                              static_cast<int>(GetPromptType())).toInt()
                              ));
    SetSavePassword(set.value("Manage/Password/Save", GetSavePassword()).toBool());
    SetViewPassowrd(set.value("Manage/Password/View", GetViewPassowrd()).toBool());
    return 0;
}

int CParameterViewer::Save(QSettings& set)
{
    set.setValue("Viewer/Hook/Keyboard", GetHookKeyboard());
    set.setValue("Manage/Password/Prompty/Type",
                 static_cast<int>(GetPromptType()));
    set.setValue("Manage/Password/Save", GetSavePassword());
    set.setValue("Manage/Password/View", GetViewPassowrd());
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

const QString &CParameterViewer::GetEncryptKey() const
{
    return m_szEncryptKey;
}

void CParameterViewer::SetEncryptKey(const QString &newPassword)
{
    if (m_szEncryptKey == newPassword)
        return;
    m_szEncryptKey = newPassword;
    emit sigEncryptKeyChanged();
}

const bool &CParameterViewer::GetSavePassword() const
{
    return m_bSavePassword;
}

void CParameterViewer::SetSavePassword(bool NewAutoSavePassword)
{
    if (m_bSavePassword == NewAutoSavePassword)
        return;
    m_bSavePassword = NewAutoSavePassword;
    emit sigSavePasswordChanged(m_bSavePassword);
}

CParameterViewer::PromptType CParameterViewer::GetPromptType() const
{
    return m_PromptType;
}

void CParameterViewer::SetPromptType(PromptType NewPromptType)
{
    if (m_PromptType == NewPromptType)
        return;
    m_PromptType = NewPromptType;
    emit sigPromptTypeChanged(m_PromptType);
}

int CParameterViewer::GetPromptCount() const
{
    return m_nPromptCount;
}

void CParameterViewer::SetPromptCount(int NewPromptCount)
{
    if (m_nPromptCount == NewPromptCount)
        return;
    m_nPromptCount = NewPromptCount;
    emit sigPromptCountChanged(m_nPromptCount);
}

bool CParameterViewer::GetViewPassowrd() const
{
    return m_bViewPassowrd;
}

void CParameterViewer::SetViewPassowrd(bool NewViewPassowrd)
{
    if (m_bViewPassowrd == NewViewPassowrd)
        return;
    m_bViewPassowrd = NewViewPassowrd;
    emit sigViewPassowrdChanged(m_bViewPassowrd);
}
