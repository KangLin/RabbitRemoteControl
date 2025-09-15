// Author: Kang Lin <kl222@126.com>

#include <QStandardPaths>
#include <QLoggingCategory>
#include "ParameterWebBrowser.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Parameter")
CParameterWebBrowser::CParameterWebBrowser(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_bOpenPrevious(false)
{
    m_szDownloadFolder = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    SetSearchEngine("https://cn.bing.com/search?q=%s");
    SetSearchRelaceString("%s");
    QStringList searchEngines;
    searchEngines << "https://cn.bing.com/search?q=%s";
    searchEngines << "https://www.google.com/search?q=%s";
    SetSearchEngineList(searchEngines);
}

CParameterWebBrowser::~CParameterWebBrowser()
{}

int CParameterWebBrowser::OnLoad(QSettings &set)
{
    int nRet = 0;
    SetHomeUrl(set.value("Url/Home", GetHomeUrl()).toString());
    SetTabUrl(set.value("Url/Tab", GetTabUrl()).toString());
    SetOpenPrevious(set.value("OpenPrevious/Enable", GetOpenPrevious()).toBool());
    SetDownloadFolder(set.value("Download/Folder", GetDownloadFolder()).toString());
    SetSearchEngine(set.value("SearchEngine", GetSearchEngine()).toString());
    SetSearchRelaceString(set.value("SearchEngine/SearchEngine", GetSearchRelaceString()).toString());
    SetSearchEngineList(set.value("SearchEngine/List", GetSearchEngineList()).toStringList());
    return nRet;
}

int CParameterWebBrowser::OnSave(QSettings &set)
{
    int nRet = 0;
    set.setValue("Url/Home", GetHomeUrl());
    set.setValue("Url/Tab", GetTabUrl());
    set.setValue("OpenPrevious/Enable", GetOpenPrevious());
    set.setValue("Download/Folder", GetDownloadFolder());
    set.setValue("SearchEngine", GetSearchEngine());
    set.setValue("SearchEngine/SearchEngine", GetSearchRelaceString());
    set.setValue("SearchEngine/List", GetSearchEngineList());
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

QString CParameterWebBrowser::GetDownloadFolder()
{
    return m_szDownloadFolder;
}

int CParameterWebBrowser::SetDownloadFolder(const QString& folder)
{
    if(m_szDownloadFolder == folder)
        return 0;
    m_szDownloadFolder = folder;
    SetModified(true);
    return 0;
}

QString CParameterWebBrowser::GetSearchEngine() const
{
    return m_szSearchEngine;
}

void CParameterWebBrowser::SetSearchEngine(const QString &newSearchEngine)
{
    if(m_szSearchEngine == newSearchEngine)
        return;
    m_szSearchEngine = newSearchEngine;
    SetModified(true);
}

QString CParameterWebBrowser::GetSearchRelaceString() const
{
    return m_szSearchRelaceString;
}

void CParameterWebBrowser::SetSearchRelaceString(const QString &newSearchRelaceString)
{
    if(m_szSearchRelaceString == newSearchRelaceString)
        return;
    m_szSearchRelaceString = newSearchRelaceString;
    SetModified(true);
}

QStringList CParameterWebBrowser::GetSearchEngineList() const
{
    return m_SearchEngineList;
}

void CParameterWebBrowser::SetSearchEngineList(const QStringList &newSearchEngineList)
{
    if(m_SearchEngineList == newSearchEngineList)
        return;
    m_SearchEngineList = newSearchEngineList;
    SetModified(true);
}
