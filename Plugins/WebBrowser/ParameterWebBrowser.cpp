// Author: Kang Lin <kl222@126.com>

#include <QLoggingCategory>
#include "ParameterWebBrowser.h"
static Q_LOGGING_CATEGORY(log, "WebBrowser.Parameter")

CParameterWebBrowser::CParameterWebBrowser(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_bOpenPrevious(false)
{}

CParameterWebBrowser::~CParameterWebBrowser()
{}

int CParameterWebBrowser::OnLoad(QSettings &set)
{
    int nRet = 0;
    SetHomeUrl(set.value("Url/Home", GetHomeUrl()).toString());
    SetTabUrl(set.value("Url/Tab", GetTabUrl()).toString());
    SetOpenPrevious(set.value("OpenPrevious", GetOpenPrevious()).toBool());
    return nRet;
}

int CParameterWebBrowser::OnSave(QSettings &set)
{
    int nRet = 0;
    set.setValue("Url/Home", GetHomeUrl());
    set.setValue("Url/Tab", GetTabUrl());
    set.setValue("OpenPrevious", GetOpenPrevious());
    return nRet;
}

void CParameterWebBrowser::slotSetGlobalParameters()
{
}

QString CParameterWebBrowser::GetHomeUrl()
{
    return m_szHomeUrl;
}

int CParameterWebBrowser::SetHomeUrl(const QString& url)
{
    if(m_szHomeUrl == url)
        return 0;
    m_szHomeUrl = url;
    SetModified(true);
    return 0;
}

QString CParameterWebBrowser::GetTabUrl()
{
    return m_szTabUrl;
}

int CParameterWebBrowser::SetTabUrl(const QString& url)
{
    if(m_szTabUrl == url)
        return 0;
    m_szTabUrl = url;
    SetModified(true);
    return 0;
}

bool CParameterWebBrowser::GetOpenPrevious()
{
    return m_bOpenPrevious;
}

void CParameterWebBrowser::SetOpenPrevious(bool bOpen)
{
    if(m_bOpenPrevious == bOpen)
        return;
    m_bOpenPrevious = bOpen;
    SetModified(true);
}
