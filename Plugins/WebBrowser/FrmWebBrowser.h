// Author: Kang Lin <kl222@126.com>

#pragma once

#include <QMenuBar>
#include <QMenu>
#include <QWidget>
#include <QWebEngineView>
#include <QToolBar>
#include <QLineEdit>
#include <QProgressBar>
#include <QTabWidget>
#include <QTimer>

#include "FrmWebView.h"
#include "FrmDownloadManager.h"
#include "ParameterWebBrowser.h"
#include "MultimediaRecord.h"
#include "History/HistoryDatabase.h"
#include "Bookmark/BookmarkDatabase.h"

class CFrmWebBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit CFrmWebBrowser(CParameterWebBrowser* pPara, bool bMenuBar = false, QWidget *parent = nullptr);
    virtual ~CFrmWebBrowser();

    enum RV{
        Success = 0,
        Fail = -1,
        FailCapturePage = -2,
        FailCaptureFullPage = -3,
        FailRecordPage = -4
    };
    Q_ENUM(RV)

    int Load(QSettings &set);
    int Save(QSettings &set);
    int Start();
    int Stop();
    QWebEngineView *CreateWindow(QWebEnginePage::WebWindowType type, bool offTheRecord = false);
    [[nodiscard]] virtual QMenu* GetMenu(QWidget* parent = nullptr);

Q_SIGNALS:
    void sigInformation(const QString& szInfo);
    void sigError(const int nError, const QString &szError);
    //! Title or icon changed
    void sigUpdateTitle();
    void sigFullScreen(bool bFull);

private Q_SLOTS:
    void slotTabCurrentChanged(int index);
    void slotTabCloseRequested(int index);
    void slotViewCloseRequested();
    void slotReturnPressed();
    void slotUrlSelected(const QString& szUrl);
    void slotInspector(bool checked);

    void slotPrint();
    void slotPrintFinished(bool success);
    void slotPrintToPdf();
    void slotPdfPrintingFinished(const QString& szFile, bool success);

    void slotCapturePage();
    void slotCaptureFullPage();
    void slotRecord();
    void slotRecordTimeout();

    void slotAddBookmark();

public Q_SLOTS:
    void slotFullScreen(bool bFullScreen);

private:
    int InitMenu(QMenu* pMenu);
    void EnableAction(bool enable);
    void SetConnect(CFrmWebView* pWeb);
    [[nodiscard]] QWebEngineProfile* GetProfile(bool offTheRecord = false);
    [[nodiscard]] CFrmWebView *CreateWebView(bool offTheRecord = false);
    [[nodiscard]] QWidget* CreateTab(/*[in/out]*/CFrmWebView **view = nullptr, bool offTheRecord = false);
    enum class ViewType{
        Web = 0,
        DevTools = 1
    };
    [[nodiscard]] CFrmWebView *CurrentView(ViewType type = ViewType::Web);
    [[nodiscard]] bool IsCurrentView(CFrmWebView *pView);
    [[nodiscard]] int IndexOfTab(CFrmWebView* pView);
    [[nodiscard]] CFrmWebView* GetView(int index, ViewType type = ViewType::Web);

private:
    CParameterWebBrowser* m_pPara;
    QMenuBar* m_pMenuBar;
    QMenu m_Menu;
    QToolBar* m_pToolBar;
    QAction* m_pBack;
    QAction* m_pForward;
    QAction* m_pRefresh;
    QAction* m_pStop;
    QAction* m_pAddBookmark;
    QAction* m_pFind;
    QAction* m_pFindNext;
    QAction* m_pFindPrevious;
    QAction* m_pZoomOriginal;
    QAction* m_pZoomIn;
    QAction* m_pZoomOut;
    QAction* m_pFavAction;
    QAction* m_pGo;
    QAction* m_pAddPage;
    QAction* m_pAddPageIncognito;
    QAction* m_pAddWindow;
    QAction* m_pAddWindowIncognito;
    QAction* m_pPrint;
    QAction* m_pPrintToPdf;
    QAction* m_pDownload;
    QAction* m_pInspector;
    QAction* m_pCapturePage;
    QAction* m_pCaptureFulPage;
    QAction* m_pRecord;
    QAction* m_pUrl;
    QLineEdit* m_pUrlLineEdit;
    QProgressBar* m_pProgressBar;
    QTabWidget* m_pTab;
    QScopedPointer<QWebEngineProfile> m_profile;
    CFrmDownloadManager m_DownloadManager;
    QString m_szFindText;

    QString m_szStyleSheet;
    friend CFrmWebView;

    CMultimediaRecordThread* m_pMultimediaRecord;
    QTimer m_tmRecord;

    CHistoryDatabase* m_pHistoryDatabase;
    CBookmarkDatabase* m_pBookmarkDatabase;
};
