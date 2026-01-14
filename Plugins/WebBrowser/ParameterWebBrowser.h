// Author: Kang Lin <kl222@126.com>

#pragma once
#include "ParameterOperate.h"
#include "ParameterRecord.h"
#include "ParameterMediaDevices.h"

class CParameterWebBrowser : public CParameterOperate
{
    Q_OBJECT
public:
    explicit CParameterWebBrowser(QObject *parent = nullptr,
                                  const QString& szPrefix = QString());
    virtual ~CParameterWebBrowser();

    CParameterRecord m_Record;
    CParameterMediaDevices m_MediaDevices;

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
public:
    bool GetPromptPrintFinished() const;
    void SetPromptPrintFinished(bool newPromptPrintFinished);
private:
    bool m_bPromptPrintFinished;
public:
    bool GetAutoFillUserAndPassword() const;
    void SetAutoFillUserAndPassword(bool newAutoFillUserAndPassword);
private:
    bool m_bAutoFillUserAndPassword;

public:
    int GetDatabaseViewLimit() const;
    void SetDatabaseViewLimit(int newLimit);
private:
    int m_nDatabaseViewLimit;

public:
    QSize GetWindowSize() const;
    void SetWindowSize(const QSize &newWindowSize);
private:
    QSize m_WindowSize;

public:
    int GetBookmarkCurrentFolder() const;
    void SetBookmarkCurrentFolder(int newCurrentBookmarkFolder);
private:
    int m_nBookmarkCurrentFolder;

public:
    bool GetBookmarkShowEditor() const;
    void SetBookmarkShowEditor(bool newBookmarkShowEditor);
private:
    bool m_bBookmarkShowEditor;

    // CParameter interface
protected:
    virtual int OnLoad(QSettings &set) override;
    virtual int OnSave(QSettings &set) override;

    // CParameterOperate interface
protected slots:
    virtual void slotSetGlobalParameters() override;
};
