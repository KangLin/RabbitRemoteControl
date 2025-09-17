// Author: Kang Lin <kl222@126.com>

#include <QStandardPaths>
#include <QLoggingCategory>
#include <QWebEngineProfile>
#include "ParameterWebBrowser.h"

static Q_LOGGING_CATEGORY(log, "WebBrowser.Parameter")
CParameterWebBrowser::CParameterWebBrowser(QObject *parent, const QString &szPrefix)
    : CParameterOperate{parent, szPrefix}
    , m_bOpenPrevious(false)
    , m_bShowDownloadManager(false)
    , m_bShowDownloadLocation(false)
    , m_ClearHttpCache(false)
    , m_ClearCookie(false)
{
    //m_szDownloadFolder = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    SetDownloadFolder(QWebEngineProfile::defaultProfile()->downloadPath());
    qDebug(log) << "Download folder:" << GetDownloadFolder();
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
    SetShowDownloadManager(set.value("Download/Show/Manager", GetShowDownloadManager()).toBool());
    SetShowDownloadLocation(set.value("Download/Show/Location", GetShowDownloadLocation()).toBool());
    SetClearHttpCache(set.value("Clear/HttpCache", GetClearHttpCache()).toBool());
    SetClearCookie(set.value("Clear/Cookie", GetClearCookie()).toBool());
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
    set.setValue("Download/Show/Manager", GetShowDownloadManager());
    set.setValue("Download/Show/Location", GetShowDownloadLocation());
    set.setValue("Clear/HttpCache", GetClearHttpCache());
    set.setValue("Clear/Cookie", GetClearCookie());
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
    emit sigDownloadFolderChanged();
    return 0;
}

bool CParameterWebBrowser::GetShowDownloadManager() const
{
    return m_bShowDownloadManager;
}

void CParameterWebBrowser::SetShowDownloadManager(bool newShowDownloadManager)
{
    if(m_bShowDownloadManager == newShowDownloadManager)
        return;
    m_bShowDownloadManager = newShowDownloadManager;
    SetModified(true);
}

bool CParameterWebBrowser::GetShowDownloadLocation() const
{
    return m_bShowDownloadLocation;
}

void CParameterWebBrowser::SetShowDownloadLocation(bool newShowDownloadLocation)
{
    if(m_bShowDownloadLocation == newShowDownloadLocation)
        return;
    m_bShowDownloadLocation = newShowDownloadLocation;
    SetModified(true);
}

bool CParameterWebBrowser::GetClearCookie() const
{
    return m_ClearCookie;
}

void CParameterWebBrowser::SetClearCookie(bool newClearCookie)
{
    if(m_ClearCookie == newClearCookie)
        return;
    m_ClearCookie = newClearCookie;
    SetModified(true);
}

bool CParameterWebBrowser::GetClearHttpCache() const
{
    return m_ClearHttpCache;
}

void CParameterWebBrowser::SetClearHttpCache(bool newClearHttpCache)
{
    if(m_ClearHttpCache == newClearHttpCache)
        return;
    m_ClearHttpCache = newClearHttpCache;
    SetModified(true);
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
