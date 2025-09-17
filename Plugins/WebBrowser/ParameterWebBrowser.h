// Author: Kang Lin <kl222@126.com>

#pragma once
#include <ParameterOperate.h>

class CParameterWebBrowser : public CParameterOperate
{
    Q_OBJECT
public:
    explicit CParameterWebBrowser(QObject *parent = nullptr,
                                  const QString& szPrefix = QString());
    virtual ~CParameterWebBrowser();
public:
    QString GetHomeUrl();
    int SetHomeUrl(const QString& url);
private:
    QString m_szHomeUrl;
public:
    QString GetTabUrl();
    int SetTabUrl(const QString& url);
private:
    QString m_szTabUrl;
public:
    bool GetOpenPrevious();
    void SetOpenPrevious(bool bOpen);
private:
    bool m_bOpenPrevious;

public:
    QString GetDownloadFolder();
    int SetDownloadFolder(const QString& folder);
Q_SIGNALS:
    void sigDownloadFolderChanged();
private:
    QString m_szDownloadFolder;
public:
    bool GetShowDownloadManager() const;
    void SetShowDownloadManager(bool newShowDownloadManager);    
private:
    bool m_bShowDownloadManager;
public:
    bool GetShowDownloadLocation() const;
    void SetShowDownloadLocation(bool newShowDownloadLocation);
private:
    bool m_bShowDownloadLocation;

public:
    bool GetClearHttpCache() const;
    void SetClearHttpCache(bool newClearHttpCache);
private:
    bool m_ClearHttpCache;
public:
    bool GetClearCookie() const;
    void SetClearCookie(bool newClearCookie);
private:
    bool m_ClearCookie;

public:
    QString GetSearchEngine() const;
    void SetSearchEngine(const QString &newSearchEngine);
private:
    QString m_szSearchEngine;
public:
    QString GetSearchRelaceString() const;
    void SetSearchRelaceString(const QString &newSearchRelaceString);
private:
    QString m_szSearchRelaceString;
public:
    QStringList GetSearchEngineList() const;
    void SetSearchEngineList(const QStringList &newSearchEngineList);
private:
    QStringList m_SearchEngineList;
    
    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

    // CParameterOperate interface
protected slots:
    virtual void slotSetGlobalParameters() override;
};
